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


load("@bazel_skylib//rules:common_settings.bzl", "bool_flag")

bool_flag(
    name = "enable_intel_gpu",
    build_setting_default = True,
)

bool_flag(
    name = "enable_intel_vpu",
    build_setting_default = False,
)

bool_flag(
    name = "enable_hetero",
    build_setting_default = False,
)

bool_flag(
    name = "enable_auto_batch",
    build_setting_default = False,
)

bool_flag(
    name = "enable_read_onnx",
    build_setting_default = False,
)

bool_flag(
    name = "enable_read_paddle",
    build_setting_default = False,
)

bool_flag(
    name = "enable_read_tensorflow",
    build_setting_default = False,
)

config_setting(
    name = "gpu_enabled",
    flag_values = {"enable_intel_gpu": "true"},
)

config_setting(
    name = "vpu_enabled",
    flag_values = {"enable_intel_vpu": "true"},
)

config_setting(
    name = "hetero_enabled",
    flag_values = {"enable_hetero": "true"},
)

config_setting(
    name = "auto_batch_enabled",
    flag_values = {"enable_auto_batch": "true"},
)

config_setting(
    name = "onnx_frontend_enabled",
    flag_values = {"enable_read_onnx": "true"},
)

config_setting(
    name = "paddle_frontend_enabled",
    flag_values = {"enable_read_paddle": "true"},
)

config_setting(
    name = "tf_frontend_enabled",
    flag_values = {"enable_read_tensorflow": "true"},
)

config_setting(
    name = "opt_build",
    values = {"compilation_mode": "opt"},
)

config_setting(
    name = "dbg_build",
    values = {"compilation_mode": "dbg"},
)

filegroup(
    name = "ie_c_api",
    srcs = [
        "runtime/include/ie/c_api/ie_c_api.h",
    ],
    visibility = ["//visibility:public"],
)

filegroup(
    name = "ie_api",
    srcs = glob(
        include = [
            "runtime/include/ie/**/*.h",
            "runtime/include/ie/**/*.hpp",
            "runtime/include/ngraph/**/*.h",
            "runtime/include/ngraph/**/*.hpp",
        ],
        exclude = ["runtime/include/ie/c_api/ie_c_api.h"],
    ),
    visibility = ["//visibility:public"],
)

filegroup(
    name = "api_v2",
    srcs = glob(
        include = [
            "runtime/include/openvino/**/*.h",
            "runtime/include/openvino/**/*.hpp",
        ],
    ),
    visibility = ["//visibility:public"],
)

filegroup(
    name = "tbb_api",
    srcs = glob(["runtime/3rdparty/tbb/include/**/*.h"]),
    visibility = ["//visibility:public"],
)

filegroup(
    name = "plugins_xml",
    srcs = select({
        ":dbg_build": ["runtime/bin/intel64/Debug/plugins.xml"],
        "//conditions:default": ["runtime/bin/intel64/Release/plugins.xml"],
    }),
    visibility = ["//visibility:public"],
)

cc_library(
    name = "tbb",
    srcs = select({
        ":dbg_build": [
            "runtime/3rdparty/tbb/lib/tbb_debug.lib",
            "runtime/3rdparty/tbb/bin/tbb_debug.dll",
            "runtime/3rdparty/tbb/bin/tbb_preview_debug.dll",
            "runtime/3rdparty/tbb/bin/tbbbind_debug.dll",
            "runtime/3rdparty/tbb/bin/tbbmalloc_debug.dll",
        ],
        "//conditions:default": [
            "runtime/3rdparty/tbb/lib/tbb.lib",
            "runtime/3rdparty/tbb/bin/tbb.dll",
            "runtime/3rdparty/tbb/bin/tbb_preview.dll",
            "runtime/3rdparty/tbb/bin/tbbbind.dll",
            "runtime/3rdparty/tbb/bin/tbbmalloc.dll",
        ],
    }),
    hdrs = [":tbb_api"],
    includes = ["runtime/3rdparty/tbb/include"],
    linkstatic = True,
    visibility = ["//visibility:public"],
)

cc_library(
    name = "ir_frontend",
    srcs = select({
        ":dbg_build": [
            "runtime/bin/intel64/Debug/openvino_ir_frontendd.dll",
        ],
        "//conditions:default": [
            "runtime/bin/intel64/Release/openvino_ir_frontend.dll",
        ],
    }),
    linkstatic = True,
    visibility = ["//visibility:public"],
)

cc_library(
    name = "onnx_frontend",
    srcs = select({
        ":dbg_build": [
            "runtime/bin/intel64/Debug/openvino_onnx_frontendd.dll",
        ],
        "//conditions:default": [
            "runtime/bin/intel64/Release/openvino_onnx_frontend.dll",
        ],
    }),
    linkstatic = True,
    visibility = ["//visibility:public"],
)

cc_library(
    name = "paddle_frontend",
    srcs = select({
        ":dbg_build": [
            "runtime/bin/intel64/Debug/openvino_paddle_frontendd.dll",
        ],
        "//conditions:default": [
            "runtime/bin/intel64/Release/openvino_paddle_frontend.dll",
        ],
    }),
    linkstatic = True,
    visibility = ["//visibility:public"],
)

cc_library(
    name = "tensorflow_frontend",
    srcs = select({
        ":dbg_build": [
            "runtime/bin/intel64/Debug/openvino_tensorflow_fed.dll",
        ],
        "//conditions:default": [
            "runtime/bin/intel64/Release/openvino_tensorflow_fe.dll",
        ],
    }),
    linkstatic = True,
    visibility = ["//visibility:public"],
)

cc_library(
    name = "frontend",
    visibility = ["//visibility:public"],
    deps = [":ir_frontend"] + select({
        ":onnx_frontend_enabled": [":onnx_frontend"],
        "//conditions:default": [],
    }) + select({
        ":paddle_frontend_enabled": [":paddle_frontend"],
        "//conditions:default": [],
    }) + select({
        ":tf_frontend_enabled": [":tensorflow_frontend"],
        "//conditions:default": [],
    }),
)

cc_library(
    name = "intel_cpu_plugin",
    srcs = select({
        ":dbg_build": [
            "runtime/bin/intel64/Debug/openvino_intel_cpu_plugind.dll",
        ],
        "//conditions:default": [
            "runtime/bin/intel64/Release/openvino_intel_cpu_plugin.dll",
        ],
    }),
    linkstatic = True,
    visibility = ["//visibility:public"],
)

cc_library(
    name = "intel_gpu_plugin",
    srcs = select({
        ":dbg_build": [
            "runtime/bin/intel64/Debug/openvino_intel_gpu_plugind.dll",
        ],
        "//conditions:default": [
            "runtime/bin/intel64/Release/openvino_intel_gpu_plugin.dll",
        ],
    }),
    linkstatic = True,
    visibility = ["//visibility:public"],
)

cc_library(
    name = "intel_vpu_plugin",
    srcs = select({
        ":dbg_build": [
            "runtime/bin/intel64/Debug/openvino_intel_vpux_plugind.dll",
        ],
        "//conditions:default": [
            "runtime/bin/intel64/Release/openvino_intel_vpux_plugin.dll",
        ],
    }),
    linkstatic = True,
    visibility = ["//visibility:public"],
    deps = [
        ":intel_vpu_plugin_level_zero_backend",
        ":intel_vpu_driver_compiler_adapter",
    ],
)

cc_library(
    name = "intel_vpu_plugin_level_zero_backend",
    srcs = select({
        ":dbg_build": [
            "runtime/bin/intel64/Debug/vpux_level_zero_backendd.dll",
        ],
        "//conditions:default": [
            "runtime/bin/intel64/Release/vpux_level_zero_backend.dll",
        ],
    }),
    linkstatic = True,
    visibility = ["//visibility:public"],
)

cc_library(
    name = "intel_vpu_driver_compiler_adapter",
    srcs = select({
        ":dbg_build": [
            "runtime/bin/intel64/Debug/vpux_driver_compiler_adapterd.dll",
        ],
        "//conditions:default": [
            "runtime/bin/intel64/Release/vpux_driver_compiler_adapter.dll",
        ],
    }),
    linkstatic = True,
    visibility = ["//visibility:public"],
)

cc_library(
    name = "hetero_plugin",
    srcs = select({
        ":dbg_build": [
            "runtime/bin/intel64/Debug/openvino_hetero_plugind.dll",
        ],
        "//conditions:default": [
            "runtime/bin/intel64/Release/openvino_hetero_plugin.dll",
        ],
    }),
    linkstatic = True,
    visibility = ["//visibility:public"],
    deps = [":intel_cpu_plugin"] + select({
        ":gpu_enabled": [":intel_gpu_plugin"],
        "//conditions:default": [],
    }) + select({
        ":vpu_enabled": [":intel_vpu_plugin"],
        "//conditions:default": [],
    }),
)

cc_library(
    name = "auto_batch_plugin",
    srcs = select({
        ":dbg_build": [
            "runtime/bin/intel64/Debug/openvino_auto_batch_plugind.dll",
        ],
        "//conditions:default": [
            "runtime/bin/intel64/Release/openvino_auto_batch_plugin.dll",
        ],
    }),
    linkstatic = True,
    visibility = ["//visibility:public"],
    deps = [":intel_cpu_plugin"] + select({
        ":gpu_enabled": [":intel_gpu_plugin"],
        "//conditions:default": [],
    }) + select({
        ":vpu_enabled": [":intel_vpu_plugin"],
        "//conditions:default": [],
    }),
)

cc_library(
    name = "auto_plugin",
    srcs = select({
        ":dbg_build": [
            "runtime/bin/intel64/Debug/openvino_auto_plugind.dll",
        ],
        "//conditions:default": [
            "runtime/bin/intel64/Release/openvino_auto_plugin.dll",
        ],
    }),
    linkstatic = True,
    visibility = ["//visibility:public"],
    deps = [":intel_cpu_plugin"] + select({
        ":gpu_enabled": [":intel_gpu_plugin"],
        "//conditions:default": [],
    }) + select({
        ":vpu_enabled": [":intel_vpu_plugin"],
        "//conditions:default": [],
    }),
)

cc_library(
    name = "inference_engine_c_api",
    srcs = select({
        ":dbg_build": [
            "runtime/lib/intel64/Debug/openvino_cd.lib",
            "runtime/bin/intel64/Debug/openvino_cd.dll",
        ],
        "//conditions:default": [
            "runtime/lib/intel64/Release/openvino_c.lib",
            "runtime/bin/intel64/Release/openvino_c.dll",
        ],
    }),
    hdrs = [":ie_c_api"],
    includes = ["runtime/include"],
    linkstatic = True,
    visibility = ["//visibility:public"],
    deps = [
        ":inference_engine",
        ":tbb",
    ],
)

cc_library(
    name = "inference_engine",
    srcs = select({
        ":dbg_build": [
            "runtime/lib/intel64/Debug/openvinod.lib",
            "runtime/bin/intel64/Debug/openvinod.dll",
        ],
        "//conditions:default": [
            "runtime/lib/intel64/Release/openvino.lib",
            "runtime/bin/intel64/Release/openvino.dll",
        ],
    }),
    hdrs = [":ie_api"],
    includes = [
        "runtime/include",
        "runtime/include/ie",
    ],
    linkstatic = True,
    visibility = ["//visibility:public"],
    deps = [
        ":tbb",
        ":auto_plugin",
        ":frontend",
    ] + select({
        ":auto_batch_enabled": [":auto_batch_plugin"],
        "//conditions:default": [],
    }) + select({
        ":hetero_enabled": [":hetero_plugin"],
        "//conditions:default": [],
    }),
)

cc_library(
    name = "openvino_runtime",
    srcs = select({
        ":dbg_build": [
            "runtime/lib/intel64/Debug/openvinod.lib",
            "runtime/bin/intel64/Debug/openvinod.dll",
        ],
        "//conditions:default": [
            "runtime/lib/intel64/Release/openvino.lib",
            "runtime/bin/intel64/Release/openvino.dll",
        ],
    }),
    hdrs = [
        ":api_v2",
        ":ie_api",
    ],
    data = select({
        ":dbg_build": [
            "runtime/bin/intel64/Debug/cache.json",
            "runtime/bin/intel64/Debug/plugins.xml",
        ],
        "//conditions:default": [
            "runtime/bin/intel64/Release/cache.json",
            "runtime/bin/intel64/Release/plugins.xml",
        ],
    }),
    includes = [
        "runtime/include",
        "runtime/include/ie",
    ],
    linkstatic = True,
    visibility = ["//visibility:public"],
)

cc_library(
    name = "openvino",
    visibility = ["//visibility:public"],
    deps = [
        ":tbb",
        ":auto_plugin",
        ":frontend",
        ":openvino_runtime",
    ] + select({
        ":auto_batch_enabled": [":auto_batch_plugin"],
        "//conditions:default": [],
    }) + select({
        ":hetero_enabled": [":hetero_plugin"],
        "//conditions:default": [],
    }),
)
