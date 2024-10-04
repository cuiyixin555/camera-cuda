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


load("@subway//bazel/subway:compile_options.bzl", "SUBWAY_DEFAULT_COPTS")

package(default_visibility = ["//visibility:public"])

licenses(["notice"])

cc_library(
    name = "cl_runtime",
    srcs = [":src/cl_function_list.inc"] + select({
        "@platforms//os:windows": [":src/cl_runtime_win32.cpp"],
        "//conditions:default": [],
    }),
    hdrs = glob(["include/**"]),
    copts = SUBWAY_DEFAULT_COPTS,
    defines = ["CL_TARGET_OPENCL_VERSION=300"],
    includes = ["include"],
)
