"""
Copyright (c) 2022 Intel Corporation
Author: Wenyi Tang
E-mail: wenyi.tang@intel.com

Windows RC compiler
"""

def _rc_compile_impl(ctx):
    """Compile .rc resource file

    > rc /l 0x0409 /nologo /fo"resource.res" resource.rc
    """

    if "not found" in ctx.attr._rc:
        return [CcInfo()]

    flags = ["/D{}".format(k) for k, v in ctx.attr.defines.items() if str(v).empty()]
    flags += ["/D{}={}".format(k, v) for k, v in ctx.attr.defines.items() if not str(v).empty()]
    includes = {}
    for fd in ctx.files.hdrs:
        includes[fd.dirname] = 1
    if includes:
        flags += ["/I{}".format(i) for i in includes]

    out_res = []
    for src in ctx.files.srcs:
        res = ctx.actions.declare_file(src.basename[:-len(".rc")] + ".res")
        src_filled = ctx.actions.declare_file(src.basename)
        ctx.actions.expand_template(
            template = src,
            output = src_filled,
            substitutions = ctx.attr.substitutions,
        )
        ctx.actions.run(
            outputs = [res],
            inputs = [src_filled] + ctx.files.hdrs + ctx.files.data,
            executable = ctx.attr._rc,
            arguments = flags + [
                "/nologo",
                "/fo{}".format(res.path),
                src_filled.path,
            ],
            env = {
                "INCLUDE": ctx.attr._system,
            },
            mnemonic = "WindowsRc",
        )
        out_res.append(res)
    link_flags = [res.path for res in out_res]
    linker_inputs = cc_common.create_linker_input(
        owner = ctx.label,
        additional_inputs = depset(out_res),
        user_link_flags = link_flags,
    )
    linking_context = cc_common.create_linking_context(linker_inputs = depset([linker_inputs]))
    return [
        DefaultInfo(files = depset(out_res)),
        CcInfo(linking_context = linking_context),
    ]

rc_library = rule(
    implementation = _rc_compile_impl,
    attrs = {
        "srcs": attr.label_list(
            allow_files = [".rc", ".rc.in"],
            mandatory = True,
            doc = """resource .rc files.""",
        ),
        "hdrs": attr.label_list(
            allow_empty = True,
            allow_files = [".h", ".hpp", ".inc", ".inl"],
            doc = """resource headers.""",
        ),
        "data": attr.label_list(
            allow_empty = True,
            doc = """data files included in the resource file.""",
        ),
        "defines": attr.string_dict(
            allow_empty = True,
            doc = """Add macro definitions.
The key: value in the dict will be passed as /Dkey=value to rc compiler.
""",
        ),
        "substitutions": attr.string_dict(
            allow_empty = True,
        ),
        "_rc": attr.string(default = "%{rc_compiler}", doc = """Path to rc.exe compiler"""),
        "_system": attr.string(default = "%{system_include}"),
    },
    fragments = ["cpp"],
    doc = """Compiles Windows resources (RC files) to be embedded in the final cc_binary.
Accepts .rc files (with accompanying resources) and embeds them into the cc_binary that
depends on this target.

Example usage:
    rc_library(
        name = "hello_resources",
        srcs = [
            "hello.rc",
            "version.rc",
        ],
        data = [
            "version.txt",
            "//images:app.ico",
        ],
    )

    cc_binary(
        name = "hello",
        srcs = ["main.cc"],
        deps = [":hello_resources"],
    )""",
)
