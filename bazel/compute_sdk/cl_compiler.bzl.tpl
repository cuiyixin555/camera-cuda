# MIT License

# Copyright (c) 2024 Cui, Xin

# Permission is hereby granted, free of charge, to any person obtaining a copy
# of this software and associated documentation files (the "Software"), to deal
# in the Software without restriction, including without limitation the rights
# to use, copy, modify, merge, publish, distribute, sublicense, and/or sell
# copies of the Software, and to permit persons to whom the Software is
# furnished to do so, subject to the following conditions:

# The above copyright notice and this permission notice shall be included in all
# copies or substantial portions of the Software.

# THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
# IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
# FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
# AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
# LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
# OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE
# SOFTWARE.
#


load("@subway//bazel/subway:objects.bzl", "subway_cc_library")

_CL = "%{ocloc_bin}"
OCLOC = _CL
_USE_SDK = "%{use_sdk}"

FileCollectorInfo = provider(
    "Collect deps files",
    fields = {"files": "collected files"},
)

def _file_collector_aspect_impl(target, ctx):
    # This function is executed for each dependency the aspect visits.

    # Collect files from the srcs
    direct = [
        f
        for f in ctx.rule.files.src
        if ctx.attr._extension == f.extension
    ]

    # Combine direct files with the files from the dependencies.
    files = depset(
        direct = direct,
        transitive = [dep[FileCollectorInfo].files for dep in ctx.rule.attr.deps],
    )

    return [FileCollectorInfo(files = files)]

file_collector_aspect = aspect(
    implementation = _file_collector_aspect_impl,
    attr_aspects = ["deps"],
    attrs = {
        "_extension": attr.string(default = "cl"),
    },
)

def _local_exec_transition_impl(settings, attr):
    return {}

# A transition that forces all targets in the subgraph to be built locally.
_local_exec_transition = transition(
    implementation = _local_exec_transition_impl,
    inputs = [],
    outputs = [],
)

def _cl_exists(ctx):
    if ctx.attr._cl.find("NOTFOUND") != -1:
        return False
    return True

def _compile_impl(ctx):
    if not _cl_exists(ctx):
        # create an empty <wrapper>.cc and <name>.inc
        ctx.actions.write(ctx.outputs.out, "")
        ctx.actions.write(ctx.outputs.wrapper, "")
        return

    out = ctx.outputs.out
    out_bin = out.basename[:-len(out.extension)] + "bin"
    out_spv = out_bin + ".spv"
    args = ctx.actions.args()
    args.add("-file", ctx.file.src)
    args.add("-device", ctx.attr.device)
    args.add("-out_dir", out.dirname)
    args.add("-output", out_bin)
    args.add("-output_no_suffix")
    defs = dict(ctx.attr.defines)
    defs.setdefault("LC_TILE_X", 1)
    defs.setdefault("LC_TILE_Y", 1)
    defs.setdefault("LC_TILE_Z", 1)
    defs.setdefault("GB_TILE_X", 1)
    defs.setdefault("GB_TILE_Y", 1)
    defs.setdefault("GB_TILE_Z", 1)
    defines = ["{}={}".format(k, v) for k, v in defs.items() if str(v) != ""]
    defines += ["{}".format(k) for k, v in defs.items() if str(v) == ""]
    options = ["-D{}".format(d) for d in defines]
    includes = {}
    deps = []
    for dep in ctx.attr.deps:
        for f in dep[FileCollectorInfo].files.to_list():
            includes[f.dirname] = 1
            deps.append(f)
    if includes:
        options += ["-I{}".format(i) for i in includes]
    if ctx.attr.copts:
        options += ctx.attr.copts
    args.add("-options", "%s" % " ".join(options))

    # different SDK has different naming: legacy use '', latest add extra '.bin'
    if  _USE_SDK == "LATEST_SDK":
        extra_ext = ".bin"
    else:
        extra_ext = ""

    if ctx.attr.binary:
        out_file = ctx.actions.declare_file(out_bin + extra_ext)
    else:
        out_file = ctx.actions.declare_file(out_spv)
    ctx.actions.run(
        outputs = [out_file],
        inputs = [ctx.file.src] + deps,
        executable = ctx.attr._cl,
        use_default_shell_env = True,
        arguments = [args],
        progress_message = "compiling {}".format(ctx.file.src),
    )
    ctx.actions.run(
        outputs = [out],
        inputs = [out_file],
        executable = ctx.executable._bin2h.path,
        arguments = [
            out_file.dirname + "/" + out_file.basename,
            out.dirname + "/" + out.basename,
            out.basename[:-len(out.extension) - 1],
        ],
        tools = [ctx.executable._bin2h],
    )
    substitutions = {
        "${INC_HEADER}": out.basename,
        "${KNAME}": out.basename.replace(".inc", ""),
        "${BIN}": "{}::code".format(ctx.label.name),
        "@DEFINE@": ";".join(defines),
    }
    if ctx.attr.binary:
        substitutions["#cmakedefine BINARY"] = "#define BINARY"
    else:
        substitutions["#cmakedefine BINARY"] = "// #define BINARY"
    ctx.actions.expand_template(
        template = ctx.file._kernel,
        output = ctx.outputs.wrapper,
        substitutions = substitutions,
    )

_compile = rule(
    implementation = _compile_impl,
    attrs = {
        "src": attr.label(
            mandatory = True,
            allow_single_file = [".cl"],
            doc = "OpenCL kernel source, one single file",
        ),
        "out": attr.output(
            doc = "Compiled binary (.bin)",
        ),
        "wrapper": attr.output(
            doc = "Wrapper file (.cc), derived from kernel.tpl",
        ),
        "device": attr.string(
            default = "adlp",
            doc = "Target compiling device",
        ),
        "binary": attr.bool(
            default = False,
            doc = """Compile directly to platform specific binary codes.
            If `binary` is not set, compiled to SPIR-V IR.
            """,
        ),
        "defines": attr.string_dict(
            allow_empty = True,
            doc = "Add macro definitions to compiler.",
        ),
        "deps": attr.label_list(
            allow_empty = True,
            aspects = [file_collector_aspect],
            doc = "Add dependencies to this target.",
        ),
        "copts": attr.string_list(
            allow_empty = True,
            doc = "Add more compiler flags.",
        ),
        "_cl": attr.string(default = _CL),
        "_kernel": attr.label(
            allow_single_file = True,
            default = "@subway//subway/kernels:template/cl_kernel.tpl",
        ),
        "_bin2h": attr.label(
            default = "@subway//tools:bin_to_h",
            executable = True,
            cfg = _local_exec_transition,
        ),
        # Comment this line out if using bazel>=7.0.0
        "_whitelist_function_transition": attr.label(default = "@bazel_tools//tools/whitelists/function_transition_whitelist"),
    },
)

def compile(name, visibility = None, **kwargs):
    """Add an OpenCL kernel to compile.

    Compile the .cl source by ocloc.exe and generate a wrapper library to register
    kernel binary into kernel pool.

    Args:
        name: shader name.
        visibility: visibility.
        **kwargs: see _compile rule doc.
    """
    wrapper = name + ".cc"
    out = name + ".inc"
    deps = [d + "_wrapper" for d in kwargs.get("deps", [])]
    _compile(name = name, out = out, wrapper = wrapper, **kwargs)
    subway_cc_library(
        name = name + "_wrapper",
        srcs = [":" + name],
        visibility = visibility,
        deps = [
            "@subway//subway/program:kernel_pool",
        ] + deps,
        alwayslink = 1,
    )
