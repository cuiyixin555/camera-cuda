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

# imgui bazel build file for windows & dx11

load("@subway//bazel:msvc_helper.bzl", "link_libraries")

cc_library(
    name = "imgui",
    srcs = [
        "backends/imgui_impl_dx11.cpp",
        "backends/imgui_impl_win32.cpp",
    ] + glob(
        ["*.cpp"],
        exclude = ["imgui_demo.cpp"],
    ),
    hdrs = [
        "backends/imgui_impl_dx11.h",
        "backends/imgui_impl_win32.h",
    ] + glob(["*.h"]),
    includes = ["backends"],
    linkopts = link_libraries([
        "onecore.lib",
        "d3d11.lib",
        "dxgi.lib",
    ]),
    linkstatic = True,
    target_compatible_with = ["@platforms//os:windows"],
    visibility = ["//visibility:public"],
)
