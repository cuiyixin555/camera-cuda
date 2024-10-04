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


def is_windows(ctx):
    return "windows" in ctx.os.name

def suffix(ctx):
    if is_windows(ctx):
        return ".exe"
    return ""

def _local_csdk_configure(ctx, csdk):
    # Read compute sdk from environment variable
    csdk_dir = ctx.os.environ.get("COMPUTESDK_DIR")
    if csdk_dir:
        csdk["ocloc_bin"] = ctx.path(csdk_dir + "/compiler/bin/ocloc{}".format(suffix(ctx)))
        csdk["cmc_bin"] = ctx.path(csdk_dir + "/compiler/bin/cmc{}".format(suffix(ctx)))
        csdk["cm_includes"] = ctx.path(csdk_dir + "/cmemu/include/libcm")
        # consider local sdk == legacy sdk
        csdk["use_sdk"] = "LEGACY_SDK"
        return True
    else:
        return False

def _latest_csdk_configure(ctx, csdk):
    csdk["ocloc_bin"] = ctx.path(Label("@latest_compute_sdk_win32//:compiler/bin/ocloc.exe"))
    csdk["cmc_bin"] = ctx.path(Label("@latest_compute_sdk_win32//:compiler/bin/cmc.exe"))
    csdk["cm_includes"] = ctx.path(Label("@latest_compute_sdk_win32//:cmemu/include/libcm/cm/cm.h"))
    csdk["use_sdk"] = "LATEST_SDK"
    print("USE LATEST SDK")

def _legacy_csdk_configure(ctx, csdk):
    csdk["ocloc_bin"] = ctx.path(Label("@legacy_compute_sdk_win32//:compiler/bin/ocloc.exe"))
    csdk["cmc_bin"] = ctx.path(Label("@legacy_compute_sdk_win32//:compiler/bin/cmc.exe"))
    csdk["cm_includes"] = ctx.path(Label("@legacy_compute_sdk_win32//:cmemu/include/libcm/cm/cm.h"))
    csdk["use_sdk"] = "LEGACY_SDK"
    print("USE LEGACY SDK")

def _csdk_configure(ctx):
    csdk = {
        "ocloc_bin": "OCLOC-NOTFOUND",
        "cmc_bin": "CMC-NOTFOUND",
        "cm_includes": "CMC-NOTFOUND",
        "use_sdk": "SDK-NOTFOUND",
    }
    use_local_csdk=_local_csdk_configure(ctx, csdk)
    if not use_local_csdk:
        use_legacy_sdk = ctx.os.environ.get("LEGACY_SDK")
        if use_legacy_sdk == "1":
            _legacy_csdk_configure(ctx, csdk)
        else:
            _latest_csdk_configure(ctx, csdk)
    ctx.template("BUILD.bazel", Label("@//bazel/compute_sdk:BUILD.bazel"))
    ctx.template("cl_compiler.bzl", Label("@//bazel/compute_sdk:cl_compiler.bzl.tpl"), {
        "%{ocloc_bin}": str(csdk["ocloc_bin"]),
        "%{use_sdk}": str(csdk["use_sdk"]),
    })
    ctx.template("cm_compiler.bzl", Label("@//bazel/compute_sdk:cm_compiler.bzl.tpl"), {
        "%{ocloc_bin}": str(csdk["ocloc_bin"]),
        "%{cmc_bin}": str(csdk["cmc_bin"]),
        "%{cm_includes}": str(csdk["cm_includes"]),
        "%{use_sdk}": str(csdk["use_sdk"]),
    })

csdk_configure = repository_rule(
    implementation = _csdk_configure,
    environ = [
        "COMPUTESDK_DIR",
        "LEGACY_SDK",
    ],
)
