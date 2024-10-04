"""
Copyright (c) 2022 Intel Corporation
Author: Wenyi Tang
E-mail: wenyi.tang@intel.com

generate wpp trace header
"""

TRACEWPP = "%{tracewpp}"
WPPCONFIG = "%{wppconfig}"
GUID = "CBCCA12E-9472-409D-A1B1-753C98BF03C0"

def _wpp_gen_tmh(ctx):
    """Generate wpp tmh header file.

    TODO: tracewpp.exe doesn't support posix path, hence the include path
    -IC:/Program Files/... can't be recognized.

    Args:
        ctx (_type_): repository ctx
    """
    ctx.actions.run(
        outputs = [ctx.outputs.out],
        inputs = [ctx.file.src],
        executable = ctx.attr._wpp,
        arguments = [
            "-dll",
            "-I \"{}\"".format(ctx.attr._config.replace("/", "\\")),
            "-ctl:{}".format(ctx.attr._guid),
            "-odir:\"{}\"".format(ctx.outputs.out.dirname),
            ctx.file.src.path.replace("/", "\\"),
        ],
    )

wpp_gen_tmh = rule(
    implementation = _wpp_gen_tmh,
    attrs = {
        "src": attr.label(allow_single_file = True, mandatory = True),
        "out": attr.output(),
        "_wpp": attr.string(default = TRACEWPP),
        "_config": attr.string(default = WPPCONFIG),
        "_guid": attr.string(default = GUID),
    },
)

def _var_providing_rule_impl(ctx):
    """Provide customized variables for genrule
    """
    return [
        platform_common.TemplateVariableInfo({
            "TRACEWPP": ctx.attr._tracewpp,
            "WPPCONFIG": ctx.attr._wppconfig,
            "GUID": ctx.attr._guid,
        }),
    ]

var_providing_rule = rule(
    implementation = _var_providing_rule_impl,
    attrs = {
        "_tracewpp": attr.string(default = TRACEWPP),
        "_wppconfig": attr.string(default = WPPCONFIG),
        "_guid": attr.string(default = GUID),
    },
)
