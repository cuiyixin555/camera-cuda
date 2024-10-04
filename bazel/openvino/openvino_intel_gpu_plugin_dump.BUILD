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


load("@subway//bazel:msvc_helper.bzl", "link_libraries")

config_setting(
    name = "opt_build",
    values = {"compilation_mode": "opt"},
)

config_setting(
    name = "dbg_build",
    values = {"compilation_mode": "dbg"},
)

#########################################
#    openvino gpu plugin dependencies   #
#########################################

cc_library(
    name = "rapidjson",
    hdrs = glob(["src/plugins/intel_gpu/thirdparty/rapidjson/**/*.h"]),
    includes = ["src/plugins/intel_gpu/thirdparty/rapidjson"],
)

cc_library(
    name = "pugixml",
    srcs = glob(["thirdparty/pugixml/src/*.cpp"]),
    hdrs = glob(["thirdparty/pugixml/src/*.hpp"]),
    includes = ["thirdparty/pugixml/src"],
)

cc_library(
    name = "itt",
    srcs = glob(["src/common/itt/src/*.cpp"]),
    hdrs = glob(["src/common/itt/include/openvino/*.hpp"]),
    includes = ["src/common/itt/include"],
    deps = [":util"],
)

cc_library(
    name = "util",
    srcs = glob(["src/common/util/src/*.cpp"]) + select({
        "@platforms//os:windows": glob(["src/common/util/src/os/win/*.cpp"]),
        "//conditions:default": glob(["src/common/util/src/os/lin/*.cpp"]),
    }),
    hdrs = glob(["src/common/util/include/**/*.hpp"]),
    includes = ["src/common/util/include"],
)

# opencl C and C++ headers
cc_library(
    name = "cl_headers",
    hdrs = glob([
        "thirdparty/ocl/cl_headers/CL/*.h",
        "thirdparty/ocl/clhpp_headers/include/CL/*.hpp",
    ]),
    includes = [
        "thirdparty/ocl/cl_headers",
        "thirdparty/ocl/clhpp_headers/include",
    ],
)

cc_library(
    name = "OpenCL",
    linkopts = select({
        "@platforms//os:windows": [],
        "//conditions:default": ["-lOpenCL"],
    }),
    deps = [
        ":cl_headers",
    ] + select({
        "@platforms//os:windows": ["@cl_runtime"],
        "//conditions:default": [],
    }),
)

# openvino non-public api, files not in released include folder
cc_library(
    name = "dev_api",
    hdrs = glob([
        "src/inference/dev_api/**/*.h",
        "src/inference/dev_api/**/*.hpp",
        "src/core/dev_api/*.hpp",
        "src/core/include/**/*.hpp",
    ]),
    includes = [
        "src/common/preprocessing",
        "src/common/transformations/include",
        "src/common/transformations/include/transformations",
        "src/core/include",
        "src/inference/dev_api",
        "src/inference/include",
        "src/inference/include/ie",
    ],
)

#########################################
#    openvino gpu plugin components     #
#########################################

cc_library(
    name = "openvino_intel_gpu_runtime",
    srcs = glob([
        "src/plugins/intel_gpu/src/runtime/**/*.h",
        "src/plugins/intel_gpu/src/runtime/**/*.hpp",
        "src/plugins/intel_gpu/src/runtime/**/*.cpp",
    ]),
    hdrs = ["@openvino_2022_win32//:api_v2"],
    includes = [
        "src/plugins/intel_gpu/include",
        "src/plugins/intel_gpu/src/runtime",
    ],
    deps = [
        ":OpenCL",
        ":dev_api",
        ":itt",
        "@subway//bazel/openvino:hack",
    ],
)

cc_library(
    name = "kernel_codegen",
    includes = ["src/plugins/intel_gpu/include/codegen/include"],
    textual_hdrs = glob(["src/plugins/intel_gpu/include/codegen/include/*.inc"]),
)

cc_library(
    name = "openvino_intel_gpu_kernels",
    srcs = glob([
        "src/plugins/intel_gpu/src/kernel_selector/**/*.h",
        "src/plugins/intel_gpu/src/kernel_selector/**/*.hpp",
        "src/plugins/intel_gpu/src/kernel_selector/**/*.cpp",
    ]),
    includes = [
        "src/plugins/intel_gpu/include",
        "src/plugins/intel_gpu/src/kernel_selector",
        "src/plugins/intel_gpu/src/kernel_selector/common",
        "src/plugins/intel_gpu/src/kernel_selector/core",
        "src/plugins/intel_gpu/src/kernel_selector/core/actual_kernels",
        "src/plugins/intel_gpu/src/kernel_selector/core/cache",
        "src/plugins/intel_gpu/src/kernel_selector/core/common",
    ],
    deps = [
        ":OpenCL",
        ":dev_api",
        ":kernel_codegen",
        ":rapidjson",
    ],
)

cc_library(
    name = "openvino_intel_gpu_graph",
    srcs = glob(
        [
            "src/plugins/intel_gpu/src/graph/**/*.h",
            "src/plugins/intel_gpu/src/graph/**/*.hpp",
            "src/plugins/intel_gpu/src/graph/**/*.cpp",
        ],
        exclude = ["src/plugins/intel_gpu/src/graph/impls/onednn/**"],
    ),
    copts = select({
        "@subway//subway:windows-clang-cl": [
            "-mavx2",
            "-mssse3",
        ],
        "@platforms//os:windows": [],
        "@subway//subway:llvm-clang": [
            "-mavx2",
            "-mssse3",
        ],
        "//conditions:default": ["-msse3"],
    }),
    includes = [
        "src/plugins/intel_gpu/src",
        "src/plugins/intel_gpu/src/graph",
        "src/plugins/intel_gpu/src/graph/include",
    ],
    deps = [
        ":OpenCL",
        ":itt",
        ":openvino_intel_gpu_runtime",
        ":openvino_intel_gpu_kernels",
    ] + select({
        "@platforms//os:windows": [
            "@openvino_2022_win32//:openvino_runtime",
        ],
        "//conditions:default": [
            "@openvino_2022_linux//:openvino_runtime",
        ],
    }),
)

cc_binary(
    name = "openvino_intel_gpu_plugin",
    srcs = glob([
        "src/plugins/intel_gpu/src/plugin/**/*.cpp",
        "src/plugins/intel_gpu/include/intel_gpu/plugin/*.hpp",
    ]),
    defines = ["IMPLEMENT_INFERENCE_ENGINE_PLUGIN"] + select({
        ":dbg_build": ["IE_BUILD_POSTFIX=\\\"d\\\""],
        "//conditions:default": ["IE_BUILD_POSTFIX=\\\"\\\""],
    }),
    includes = [
        "src/common/low_precision_transformations/include",
        "src/plugins/intel_gpu/include",
    ],
    linkopts = select({
        "@platforms//os:windows": link_libraries(["onecore.lib"]),
        "//conditions:default": [],
    }),
    linkshared = True,
    visibility = ["//visibility:public"],
    deps = [
        ":OpenCL",
        ":itt",
        ":openvino_intel_gpu_graph",
        ":pugixml",
        ":util",
        "@subway//bazel/openvino:hack",
    ],
)
