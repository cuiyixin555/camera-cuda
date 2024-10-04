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


load("@bazel_tools//tools/cpp:windows_cc_configure.bzl", "find_vc_path", "setup_vc_env_vars")

def _hlsl_configure(ctx):
    vc_path = find_vc_path(ctx)
    if not vc_path:
        # NOTFOUND is a keyword for detecting existance of FXC/DXC
        fxc = "VC is NOTFOUND on host, please specify BAZEL_VC."
        dxc = "VC is NOTFOUND on host, please specify BAZEL_VC."
    else:
        env = setup_vc_env_vars(ctx, vc_path, ["WINDOWSSDKDIR", "WINDOWSSDKVERSION"])
        sdk_bin_path = ctx.path("{}\\bin\\{}".format(env["WINDOWSSDKDIR"], env["WINDOWSSDKVERSION"]))

        # find in environment
        if "FXC" in ctx.os.environ:
            fxc = ctx.path(ctx.os.environ["FXC"])
        else:
            fxc = ctx.path("{}\\x64\\fxc.exe".format(sdk_bin_path))
        if "DXC" in ctx.os.environ:
            dxc = ctx.path(ctx.os.environ["DXC"])
        else:
            dxc = ctx.path("{}\\x64\\dxc.exe".format(sdk_bin_path))

    ctx.template("BUILD.bazel", Label("//bazel/hlsl:BUILD.tpl"))
    ctx.template("compiler.bzl", Label("//bazel/hlsl:compiler.bzl.tpl"), {
        "%{fxc}": str(fxc),
        "%{dxc}": str(dxc),
    })

hlsl_configure = repository_rule(
    implementation = _hlsl_configure,
    environ = ["BAZEL_VC"],
)
