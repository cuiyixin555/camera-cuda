"""
Copyright (c) 2022 Intel Corporation
Author: Wenyi Tang
E-mail: wenyi.tang@intel.com

fxc/dxc directx hlsl compiler rule
"""

load("@subway//bazel/subway:objects.bzl", "subway_cc_library")

_FXC = "%{fxc}"
_DXC = "%{dxc}"
_MODELS = (
    "cs_4_1",
    "cs_5_0",
    "cs_5_1",
    "ds_5_0",
    "ds_5_1",
    "gs_4_1",
    "gs_5_0",
    "gs_5_1",
    "hs_5_0",
    "hs_5_1",
    "lib_4_1",
    "lib_5_0",
    "ps_4_1",
    "ps_5_0",
    "ps_5_1",
    "rootsig_1_0",
    "rootsig_1_1",
    "tx_1_0",
    "vs_4_1",
    "vs_5_0",
    "vs_5_1",
)

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
        "_extension": attr.string(default = "hlsl"),
    },
)

def _fxc_exists(ctx):
    if ctx.attr._fxc.find("NOTFOUND") != -1:
        return False
    return True

def _fxc_impl(ctx):
    model = ctx.attr.model
    if not model in _MODELS:
        fail("Invalid model %s, should be one of %s" % (model, _MODELS))
    if not _fxc_exists(ctx):
        ctx.actions.write(ctx.outputs.out, "")
        ctx.actions.write(ctx.outputs.wrapper, "")
        return

    out = ctx.outputs.out
    args = [
        ctx.file.src.path,
        "/E{}".format(ctx.attr.entry),
        "/T{}".format(model),
        "/nologo",
        "/WX",
        "/Ges",
        "/Zi",
        "/Zpc",
        "/Qstrip_reflect",
        "/Qstrip_debug",
        "/Fh{}".format(out.path),
        "/Vn{}".format(ctx.label.name),
    ]
    defs = dict(ctx.attr.defines)
    defs.setdefault("LC_TILE_X", 1)
    defs.setdefault("LC_TILE_Y", 1)
    defs.setdefault("LC_TILE_Z", 1)
    defs.setdefault("GB_TILE_X", 1)
    defs.setdefault("GB_TILE_Y", 1)
    defs.setdefault("GB_TILE_Z", 1)
    defines = ["{}={}".format(k, v) for k, v in defs.items() if str(v) != ""]
    defines += ["{}".format(k) for k, v in defs.items() if str(v) == ""]
    args += ["/D{}".format(d) for d in defines]
    if ctx.attr.copts:
        args += ctx.attr.copts
    includes = {}
    deps = []
    for dep in ctx.attr.deps:
        for f in dep[FileCollectorInfo].files.to_list():
            includes[f.dirname] = 1
            deps.append(f)
    if includes:
        args += ["/I{}".format(i) for i in includes]
    ctx.actions.run(
        outputs = [out],
        inputs = [ctx.file.src] + deps,
        executable = ctx.attr._fxc,
        arguments = args,
    )
    if model.startswith("vs"):
        expand_model = "hlsl_vs"
    elif model.startswith("ps"):
        expand_model = "hlsl_ps"
    else:
        expand_model = "hlsl_cs"
    ctx.actions.expand_template(
        template = ctx.file._kernel,
        output = ctx.outputs.wrapper,
        substitutions = {
            "${INC_HEADER}": out.basename,
            "${MODEL}": expand_model,
            "${KNAME}": out.basename.replace(".inc", ""),
            "${BIN}": ctx.label.name,
            "@DEFINE@": ";".join(defines),
        },
    )

_fxc = rule(
    implementation = _fxc_impl,
    attrs = {
        "src": attr.label(
            mandatory = True,
            allow_single_file = [".hlsl"],
            doc = "HLSL shader source, one single file",
        ),
        "out": attr.output(
            doc = "Compiled binary, packed into a header file (.inc)",
        ),
        "wrapper": attr.output(
            doc = "Wrapper file (.cc), derived from kernel.tpl",
        ),
        "entry": attr.string(
            default = "main",
            doc = "Name of the shader entry function.",
        ),
        "model": attr.string(
            mandatory = True,
            doc = "Shader model",
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
        "_fxc": attr.string(default = _FXC),
        "_kernel": attr.label(allow_single_file = True, default = Label("@subway//subway/kernels:template/hlsl_kernel.tpl")),
    },
)

def fxc(name, visibility = None, **kwargs):
    """Add an HLSL shader kernel to compile.

    Compile the hlsl source by fxc.exe and generate a wrapper library to register
    kernel binary into kernel pool.

    Args:
        name: shader name.
        visibility: visibility
        **kwargs: see fxc rule doc.
    """
    wrapper = name + ".cc"
    out = name + ".inc"
    deps = [d + "_wrapper" for d in kwargs.get("deps", [])]
    _fxc(name = name, out = out, wrapper = wrapper, **kwargs)
    subway_cc_library(
        name = name + "_wrapper",
        srcs = [":" + name],
        visibility = visibility,
        deps = [
            "@subway//subway/program:kernel_pool",
        ] + deps,
        alwayslink = 1,
    )
