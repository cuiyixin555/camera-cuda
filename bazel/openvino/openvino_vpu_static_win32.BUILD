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
    name = "enable_intel_vpu",
    build_setting_default = False,
    visibility = ["//visibility:public"],
)

config_setting(
    name = "vpu_enabled",
    flag_values = {"enable_intel_vpu": "true"},
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
    name = "ze_loader_api",
    srcs = glob(["runtime/3rdparty/ze_loader/include/**/*.h"]),
    visibility = ["//visibility:public"],
)

cc_library(
    name = "npu_level_zero_backend",
    srcs = select({
        ":dbg_build": [
            "runtime/lib/intel64/Debug/npu_level_zero_backendd.lib",
        ],
        "//conditions:default": [
            "runtime/lib/intel64/Release/npu_level_zero_backend.lib",
        ],
    }),
    linkstatic = True,
    visibility = ["//visibility:public"],
)

cc_library(
    name = "flatbuffers",
    srcs = select({
        ":dbg_build": [
            "runtime/lib/intel64/Debug/flatbuffersd.lib",
        ],
        "//conditions:default": [
            "runtime/lib/intel64/Release/flatbuffers.lib",
        ],
    }),
    linkstatic = True,
    visibility = ["//visibility:public"],
)

cc_library(
    name = "inference_engine_obj",
    srcs = select({
        ":dbg_build": [
            "runtime/lib/intel64/Debug/inference_engine_objd.lib",
        ],
        "//conditions:default": [
            "runtime/lib/intel64/Release/inference_engine_obj.lib",
        ],
    }),
    linkstatic = True,
    visibility = ["//visibility:public"],
)

cc_library(
    name = "openvino_runtime",
    srcs = select({
        ":dbg_build": [
            "runtime/lib/intel64/Debug/openvinod.lib",
        ],
        "//conditions:default": [
            "runtime/lib/intel64/Release/openvino.lib",
        ],
    }),
    hdrs = [
        ":api_v2",
        ":ie_api",
    ],
    includes = [
        "runtime/include",
        "runtime/include/ie",
    ],
    linkstatic = True,
    visibility = ["//visibility:public"],
)

cc_library(
    name = "openvino_ir_frontend",
    srcs = select({
        ":dbg_build": [
            "runtime/lib/intel64/Debug/openvino_ir_frontendd.lib",
        ],
        "//conditions:default": [
            "runtime/lib/intel64/Release/openvino_ir_frontend.lib",
        ],
    }),
    linkstatic = True,
    visibility = ["//visibility:public"],
)

cc_library(
    name = "pugixml",
    srcs = select({
        ":dbg_build": [
            "runtime/lib/intel64/Debug/pugixmld.lib",
        ],
        "//conditions:default": [
            "runtime/lib/intel64/Release/pugixml.lib",
        ],
    }),
    linkstatic = True,
    visibility = ["//visibility:public"],
)

cc_library(
    name = "npu_driver_compiler_adapter",
    srcs = select({
        ":dbg_build": [
            "runtime/lib/intel64/Debug/npu_driver_compiler_adapterd.lib",
        ],
        "//conditions:default": [
            "runtime/lib/intel64/Release/npu_driver_compiler_adapter.lib",
        ],
    }),
    linkstatic = True,
    visibility = ["//visibility:public"],
)

cc_library(
    name = "npu_utils",
    srcs = select({
        ":dbg_build": [
            "runtime/lib/intel64/Debug/npu_utilsd.lib",
        ],
        "//conditions:default": [
            "runtime/lib/intel64/Release/npu_utils.lib",
        ],
    }),
    linkstatic = True,
    visibility = ["//visibility:public"],
)

cc_library(
    name = "inference_engine_snippets",
    srcs = select({
        ":dbg_build": [
            "runtime/lib/intel64/Debug/openvino_snippetsd.lib",
        ],
        "//conditions:default": [
            "runtime/lib/intel64/Release/openvino_snippets.lib",
        ],
    }),
    linkstatic = True,
    visibility = ["//visibility:public"],
)

cc_library(
    name = "npu_elf",
    srcs = select({
        ":dbg_build": [
            "runtime/lib/intel64/Debug/npu_elfd.lib",
        ],
        "//conditions:default": [
            "runtime/lib/intel64/Release/npu_elf.lib",
        ],
    }),
    linkstatic = True,
    visibility = ["//visibility:public"],
)

cc_library(
    name = "openvino_builders",
    srcs = select({
        ":dbg_build": [
            "runtime/lib/intel64/Debug/openvino_buildersd.lib",
        ],
        "//conditions:default": [
            "runtime/lib/intel64/Release/openvino_builders.lib",
        ],
    }),
    linkstatic = True,
    visibility = ["//visibility:public"],
)

cc_library(
    name = "openvino_c",
    srcs = select({
        ":dbg_build": [
            "runtime/lib/intel64/Debug/openvino_cd.lib",
        ],
        "//conditions:default": [
            "runtime/lib/intel64/Release/openvino_c.lib",
        ],
    }),
    linkstatic = True,
    visibility = ["//visibility:public"],
)

cc_library(
    name = "inference_engine",
    srcs = select({
        ":dbg_build": [
            "runtime/lib/intel64/Debug/openvinod.lib",
        ],
        "//conditions:default": [
            "runtime/lib/intel64/Release/openvino.lib",
        ],
    }),
    linkstatic = True,
    visibility = ["//visibility:public"],
)

cc_library(
    name = "npu_util",
    srcs = select({
        ":dbg_build": [
            "runtime/lib/intel64/Debug/npu_utild.lib",
        ],
        "//conditions:default": [
            "runtime/lib/intel64/Release/npu_util.lib",
        ],
    }),
    linkstatic = True,
    visibility = ["//visibility:public"],
)

cc_library(
    name = "vpux_level_zero_backend",
    srcs = select({
        ":dbg_build": [
            "runtime/lib/intel64/Debug/vpux_level_zero_backendd.lib",
        ],
        "//conditions:default": [
            "runtime/lib/intel64/Release/vpux_level_zero_backend.lib",
        ],
    }),
    linkstatic = True,
    visibility = ["//visibility:public"],
)

cc_library(
    name = "openvino_itt",
    srcs = select({
        ":dbg_build": [
            "runtime/lib/intel64/Debug/openvino_ittd.lib",
        ],
        "//conditions:default": [
            "runtime/lib/intel64/Release/openvino_itt.lib",
        ],
    }),
    linkstatic = True,
    visibility = ["//visibility:public"],
)


cc_library(
    name = "openvino_reference",
    srcs = select({
        ":dbg_build": [
            "runtime/lib/intel64/Debug/openvino_referenced.lib",
        ],
        "//conditions:default": [
            "runtime/lib/intel64/Release/openvino_reference.lib",
        ],
    }),
    linkstatic = True,
    visibility = ["//visibility:public"],
)

cc_library(
    name = "openvino_shape_inference",
    srcs = select({
        ":dbg_build": [
            "runtime/lib/intel64/Debug/openvino_shape_inferenced.lib",
        ],
        "//conditions:default": [
            "runtime/lib/intel64/Release/openvino_shape_inference.lib",
        ],
    }),
    linkstatic = True,
    visibility = ["//visibility:public"],
)

cc_library(
    name = "npu_al",
    srcs = select({
        ":dbg_build": [
            "runtime/lib/intel64/Debug/npu_ald.lib",
        ],
        "//conditions:default": [
            "runtime/lib/intel64/Release/npu_al.lib",
        ],
    }),
    linkstatic = True,
    visibility = ["//visibility:public"],
)

cc_library(
    name = "npu_ngraph_transformations",
    srcs = select({
        ":dbg_build": [
            "runtime/lib/intel64/Debug/npu_ngraph_transformationsd.lib",
        ],
        "//conditions:default": [
            "runtime/lib/intel64/Release/npu_ngraph_transformations.lib",
        ],
    }),
    linkstatic = True,
    visibility = ["//visibility:public"],
)

cc_library(
    name = "openvino_pytorch_frontend",
    srcs = select({
        ":dbg_build": [
            "runtime/lib/intel64/Debug/openvino_pytorch_frontendd.lib",
        ],
        "//conditions:default": [
            "runtime/lib/intel64/Release/openvino_pytorch_frontend.lib",
        ],
    }),
    linkstatic = True,
    visibility = ["//visibility:public"],
)

cc_library(
    name = "openvino_intel_npu_plugin",
    srcs = select({
        ":dbg_build": [
            "runtime/lib/intel64/Debug/openvino_intel_npu_plugind.lib",
        ],
        "//conditions:default": [
            "runtime/lib/intel64/Release/openvino_intel_npu_plugin.lib",
        ],
    }),
    linkstatic = True,
    visibility = ["//visibility:public"],
)

cc_library(
    name = "openvino_intel_cpu_plugin",
    srcs = select({
        ":dbg_build": [
            "runtime/lib/intel64/Debug/openvino_intel_cpu_plugind.lib",
            "runtime/lib/intel64/Release/openvino_onednn_cpud.lib",
            "runtime/lib/intel64/Release/mlasd.lib",
        ],
        "//conditions:default": [
            "runtime/lib/intel64/Release/openvino_intel_cpu_plugin.lib",
            "runtime/lib/intel64/Release/openvino_onednn_cpu.lib",
            "runtime/lib/intel64/Release/mlas.lib",
        ],
    }),
    linkstatic = True,
    visibility = ["//visibility:public"],
)

cc_library(
    name = "openvino_intel_gpu_plugin",
    srcs = select({
        ":dbg_build": [
            "runtime/lib/intel64/Debug/openvino_intel_gpu_plugind.lib",
            "runtime/lib/intel64/Debug/OpenCL.lib",
        ],
        "//conditions:default": [
            "runtime/lib/intel64/Release/openvino_intel_gpu_plugin.lib",
            "runtime/lib/intel64/Release/openvino_intel_gpu_graph.lib",
            "runtime/lib/intel64/Release/openvino_intel_gpu_kernels.lib",
            "runtime/lib/intel64/Release/openvino_intel_gpu_runtime.lib",
            "runtime/lib/intel64/Release/OpenCL.lib",
        ],
    }),
    linkstatic = True,
    visibility = ["//visibility:public"],
)

cc_library(
    name = "kmb_utils",
    srcs = select({
        ":dbg_build": [
            "runtime/lib/intel64/Debug/kmb_utilsd.lib",
        ],
        "//conditions:default": [
            "runtime/lib/intel64/Release/kmb_utils.lib",
        ],
    }),
    linkstatic = True,
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
    name = "tbb12",
    srcs = select({
        ":dbg_build": [
            "runtime/3rdparty/tbb/lib/tbb12_debug.lib",
            "runtime/3rdparty/tbb/bin/tbb12_debug.dll",
        ],
        "//conditions:default": [
            "runtime/3rdparty/tbb/lib/tbb12.lib",
            "runtime/3rdparty/tbb/bin/tbb12.dll",
        ],
    }),
    hdrs = [":tbb_api"],
    includes = ["runtime/3rdparty/tbb/include"],
    linkstatic = True,
    visibility = ["//visibility:public"],
)

cc_library(
	name = "dnnl",
	srcs = select({
		":dbg_build": [
			"runtime/lib/intel64/Debug/dnnld.lib",
		],
		"//conditions:default": [
			"runtime/lib/intel64/Release/dnnl.lib",
		],
	}),
	linkstatic = True,
	visibility = ["//visibility:public"],
)

cc_library(
    name = "ze_loader",
    srcs = select({
        ":dbg_build": [
            "runtime/3rdparty/ze_loader/lib/ze_loader.lib",
            "runtime/3rdparty/ze_loader/bin/ze_loader.dll",
        ],
        "//conditions:default": [
            "runtime/3rdparty/ze_loader/lib/ze_loader.lib",
            "runtime/3rdparty/ze_loader/bin/ze_loader.dll",
        ],
    }),
    #hdrs = [":ze_loader_api"],
    includes = ["runtime/3rdparty/ze_loader/include/level_zero"],
    linkstatic = True,
    visibility = ["//visibility:public"],
)

cc_library(
    name = "npu_algo_utils",
    srcs = select({
        ":dbg_build": [
            "runtime/lib/intel64/Debug/npu_algo_utilsd.lib",
        ],
        "//conditions:default": [
            "runtime/lib/intel64/Release/npu_algo_utils.lib",
        ],
    }),
    linkstatic = True,
    visibility = ["//visibility:public"],
)

cc_library(
    name = "npu_llvm_utils",
    srcs = select({
        ":dbg_build": [
            "runtime/lib/intel64/Debug/npu_llvm_utilsd.lib",
        ],
        "//conditions:default": [
            "runtime/lib/intel64/Release/npu_llvm_utils.lib",
        ],
    }),
    linkstatic = True,
    visibility = ["//visibility:public"],
)

cc_library(
    name = "npu_model_utils",
    srcs = select({
        ":dbg_build": [
            "runtime/lib/intel64/Debug/npu_model_utilsd.lib",
        ],
        "//conditions:default": [
            "runtime/lib/intel64/Release/npu_model_utils.lib",
        ],
    }),
    linkstatic = True,
    visibility = ["//visibility:public"],
)

cc_library(
    name = "npu_ov_utils",
    srcs = select({
        ":dbg_build": [
            "runtime/lib/intel64/Debug/npu_ov_utilsd.lib",
        ],
        "//conditions:default": [
            "runtime/lib/intel64/Release/npu_ov_utils.lib",
        ],
    }),
    linkstatic = True,
    visibility = ["//visibility:public"],
)

cc_library(
    name = "npu_plugin_utils",
    srcs = select({
        ":dbg_build": [
            "runtime/lib/intel64/Debug/npu_plugin_utilsd.lib",
        ],
        "//conditions:default": [
            "runtime/lib/intel64/Release/npu_plugin_utils.lib",
        ],
    }),
    linkstatic = True,
    visibility = ["//visibility:public"],
)

cc_library(
    name = "npu_profiling_utils",
    srcs = select({
        ":dbg_build": [
            "runtime/lib/intel64/Debug/npu_profiling_utilsd.lib",
        ],
        "//conditions:default": [
            "runtime/lib/intel64/Release/npu_profiling_utils.lib",
        ],
    }),
    linkstatic = True,
    visibility = ["//visibility:public"],
)

cc_library(
    name = "openvino_util",
    srcs = select({
        ":dbg_build": [
            "runtime/lib/intel64/Debug/openvino_utild.lib",
        ],
        "//conditions:default": [
            "runtime/lib/intel64/Release/openvino_util.lib",
        ],
    }),
    linkstatic = True,
    visibility = ["//visibility:public"],
)

cc_library(
    name = "openvino",
    visibility = ["//visibility:public"],
    deps = [
        #":tbb",
        ":tbb12",
        ":flatbuffers",
        ":openvino_ir_frontend",
        ":openvino_runtime",
        #":inference_engine_obj",
        ":pugixml",
        ":inference_engine",
        #":inference_engine_snippets",
        ":openvino_builders",
        #":dnnl",
        ":openvino_c",
        # ":openvino_pytorch_frontend",
        ":openvino_util",
        ":openvino_itt",
        # ":kmb_utils",
        ":openvino_reference",
        ":openvino_shape_inference",
        ":npu_al",
        ":npu_driver_compiler_adapter",
        ":npu_level_zero_backend",
        ":npu_ngraph_transformations",
        #":npu_utils",
        ":openvino_intel_npu_plugin",
        ":npu_elf",
        ":npu_algo_utils",
        ":npu_llvm_utils",
        ":npu_model_utils",
        ":npu_ov_utils",
        ":npu_plugin_utils",
        ":npu_profiling_utils",
        ":openvino_intel_gpu_plugin",
        #":openvino_intel_cpu_plugin",
    ],
)
