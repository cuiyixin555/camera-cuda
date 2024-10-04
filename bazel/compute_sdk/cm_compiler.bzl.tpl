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

_CMC = "%{cmc_bin}"
_CM_INCLUDE = "%{cm_includes}"
_CL = "%{ocloc_bin}"
_USE_SDK = "%{use_sdk}"

def _local_exec_transition_impl(settings, attr):
    return {}

# A transition that forces all targets in the subgraph to be built locally.
# In this case build //:bin_to_h tool
_local_exec_transition = transition(
    implementation = _local_exec_transition_impl,
    inputs = [],
    outputs = [],
)

def _cmc_exists(ctx):
    if ctx.attr._cmc.find("NOTFOUND") != -1:
        return False
    return True

def _cl_exists(ctx):
    if ctx.attr._cl.find("NOTFOUND") != -1:
        return False
    return True

def _cmc_compile_to_spv(ctx):
    if not _cmc_exists(ctx):
        fail("compiler <cmc> is not found.")

    out = ctx.outputs.out
    out_spv = out.basename[:-len(out.extension)] + "cmc_spv"
    output = ctx.actions.declare_file(out_spv)
    args = ctx.actions.args()
    args.add("-emit-spirv")
    args.add("-fcmocl")
    args.add("-mcpu=lnl")
    args.add("-o", output)
    args.add("--", ctx.file.src)
    defs = dict(ctx.attr.defines)
    defs.setdefault("LC_TILE_X", 1)
    defs.setdefault("LC_TILE_Y", 1)
    defs.setdefault("LC_TILE_Z", 1)
    defs.setdefault("GB_TILE_X", 1)
    defs.setdefault("GB_TILE_Y", 1)
    defs.setdefault("GB_TILE_Z", 1)
    defines = ["{}={}".format(k, v) for k, v in defs.items() if str(v) != ""]
    defines += ["{}".format(k) for k, v in defs.items() if str(v) == ""]
    options = ["-g0"] + ["-D{}".format(d) for d in defines]
    includes = {}
    for fd in ctx.files.hdrs:
        includes[fd.dirname] = 1
    if includes:
        options += ["-I{}".format(i) for i in includes]
    if ctx.attr.copts:
        options += ctx.attr.copts

    ctx.actions.run(
        outputs = [output],
        inputs = [ctx.file.src] + ctx.files.hdrs,
        executable = ctx.attr._cmc,
        use_default_shell_env = True,
        arguments = options + [args],
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
    return output

def _cl_compile_spv_to_bin(ctx, spv):
    if not _cl_exists(ctx):
        fail("compiler <ocloc> is not found.")
    fat_binary = False
    fat_device_name = {"lnl": "xe2-lpg", "single": "xe2-lpg,xe3-lpg"}
    if ctx.attr.device in fat_device_name:
        fat_binary = True
    out = ctx.outputs.out
    out_bin = out.basename[:-len(out.extension)] + "bin"

    args = ctx.actions.args()
    args.add("-file", spv)
    args.add("-spirv_input")
    if fat_binary:
        args.add("-device", fat_device_name[ctx.attr.device])
    else:
        args.add("-device", ctx.attr.device)
    if ctx.attr.device=="lnl" or ctx.attr.device=="ptl" or ctx.attr.device=="nvl"  or ctx.attr.device=="single":
        args.add("-options", "-vc-codegen -vc-use-plain-2d-images -ftranslate-legacy-memory-intrinsics")
    else:
        args.add("-options", "-vc-codegen -vc-use-plain-2d-images")
    args.add("-out_dir", out.dirname)
    args.add("-output", out_bin)
    args.add("-output_no_suffix")

    # different SDK has different naming issue
    # if LEGACY use '', if LATEST add extra '.bin'
    if  _USE_SDK == "LATEST_SDK" and not fat_binary:
        extra_ext = ".bin"
    else:
        extra_ext = ""

    out_bin_file = ctx.actions.declare_file(out_bin + extra_ext)

    ctx.actions.run(
        outputs = [out_bin_file],
        inputs = [spv],
        executable = ctx.attr._cl,
        use_default_shell_env = True,
        arguments = [args],
    )
    return out_bin_file

def _compile_impl(ctx):
    if not _cmc_exists(ctx) or not _cl_exists(ctx):
        # create an empty <wrapper>.cc and <name>.inc
        ctx.actions.write(ctx.outputs.out, "")
        ctx.actions.write(ctx.outputs.wrapper, "")
        return

    out = ctx.outputs.out
    out_spv = _cmc_compile_to_spv(ctx)
    output = out_spv
    if ctx.attr.binary:
        out_bin = _cl_compile_spv_to_bin(ctx, out_spv)
        output = out_bin

    ctx.actions.run(
        outputs = [out],
        inputs = [output],
        executable = ctx.executable._bin2h.path,
        arguments = [
            output.dirname + "/" + output.basename,
            out.dirname + "/" + out.basename,
            out.basename[:-len(out.extension) - 1],
        ],
        tools = [ctx.executable._bin2h],
    )

_compile = rule(
    implementation = _compile_impl,
    attrs = {
        "src": attr.label(
            mandatory = True,
            allow_single_file = [".cm"],
            doc = "C-for-Metal kernel source, one single file",
        ),
        "hdrs": attr.label_list(
            allow_files = [".cm", ".h"],
            doc = "Headers need to include",
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
        "copts": attr.string_list(
            allow_empty = True,
            doc = "Add more compiler flags.",
        ),
        "_cl": attr.string(default = _CL),
        "_cmc": attr.string(default = _CMC),
        "_kernel": attr.label(
            allow_single_file = True,
            default = "@subway//subway/kernels:template/cm_kernel.tpl",
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
    """Add an C-for-Metal kernel to compile.

    Compile the .cm source by cmc.exe and generate a wrapper library to register
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
