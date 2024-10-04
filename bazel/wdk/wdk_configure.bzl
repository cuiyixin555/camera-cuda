"""
Copyright (c) 2022 Intel Corporation
Author: Wenyi Tang
E-mail: wenyi.tang@intel.com

Locate Windows Driver Kit (WDK)
"""

load("@bazel_tools//tools/cpp:windows_cc_configure.bzl", "find_vc_path", "setup_vc_env_vars")

def _wdk_configure(ctx):
    vc_path = find_vc_path(ctx)
    win_include_dir = ""
    if not vc_path:
        tracewpp = "VC is not found on host, please specify BAZEL_VC."
        wppconfig = "VC is not found on host, please specify BAZEL_VC."
        rc_compiler = "VC is not found on host, please specify BAZEL_VC."
    else:
        env = setup_vc_env_vars(ctx, vc_path, ["WINDOWSSDKDIR", "WINDOWSSDKVERSION", "INCLUDE"])
        wdk_bin_path = ctx.path("{}\\bin\\{}".format(env["WINDOWSSDKDIR"], env["WINDOWSSDKVERSION"]))
        win_include_dir = env["INCLUDE"]

        # find in environment
        if "TRACEWPP" in ctx.os.environ and "WPPCONFIG" in ctx.os.environ:
            tracewpp = ctx.path(ctx.os.environ["TRACEWPP"])
            wppconfig = ctx.path(ctx.os.environ["WPPCONFIG"])
        else:
            tracewpp = ctx.path("{}\\x64\\tracewpp.exe".format(wdk_bin_path))
            wppconfig = ctx.path("{}\\WppConfig\\Rev1".format(wdk_bin_path))

        # find in system VC path
        if not tracewpp.exists or not wppconfig.exists:
            tracewpp = ctx.path("{}\\x64\\tracewpp.exe".format(wdk_bin_path))
            wppconfig = ctx.path("{}\\WppConfig\\Rev1".format(wdk_bin_path))

        # find in camera-deps
        if not tracewpp.exists or not wppconfig.exists:
            tracewpp = ctx.path(Label("@camera_deps//:win32/wdk/10/bin/x64/tracewpp.exe"))
            wppconfig = ctx.path(Label("@camera_deps//:win32/wdk/10/bin/WppConfig/Rev1/header.tpl")).dirname

        rc_compiler = ctx.path("{}\\x64\\rc.exe".format(wdk_bin_path))

    ctx.template("BUILD.bazel", Label("//bazel/wdk:BUILD.tpl"))
    ctx.template("wpp.bzl", Label("//bazel/wdk:wpp.bzl.tpl"), {
        "%{tracewpp}": str(tracewpp),
        "%{wppconfig}": str(wppconfig),
    })
    ctx.template("rc.bzl", Label("//bazel/wdk:rc.bzl.tpl"), {
        "%{rc_compiler}": str(rc_compiler),
        "%{system_include}": win_include_dir,
    })

wdk_configure = repository_rule(
    implementation = _wdk_configure,
    environ = [
        "BAZEL_VC",
        "BAZEL_VS",
        "TRACEWPP",
        "WPPCONFIG",
    ],
)
