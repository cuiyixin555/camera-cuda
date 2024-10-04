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

package(default_visibility = ["//visibility:public"])

licenses(["notice"])

exports_files(["LICENSE"])

filegroup(
    name = "lib",
    srcs = [
        "source/lib/ze_lib.cpp",
        "source/lib/ze_lib.h",
        "source/lib/ze_libapi.cpp",
        "source/lib/ze_libddi.cpp",
        "source/lib/ze_tracing_register_cb_libapi.cpp",
        "source/lib/zel_tracing_libapi.cpp",
        "source/lib/zel_tracing_libddi.cpp",
        "source/lib/zes_libapi.cpp",
        "source/lib/zes_libddi.cpp",
        "source/lib/zet_libapi.cpp",
        "source/lib/zet_libddi.cpp",
    ] + select({
        "@platforms//os:windows": ["source/lib/windows/lib_init.cpp"],
        "//conditions:default": ["source/lib/linux/lib_init.cpp"],
    }),
    visibility = ["//visibility:private"],
)

filegroup(
    name = "loader",
    srcs = [
        "source/loader/ze_object.h",
        "source/loader/ze_loader_internal.h",
        "source/loader/ze_loader.cpp",
        "source/loader/ze_loader_api.h",
        "source/loader/ze_loader_api.cpp",
        "source/loader/ze_ldrddi.h",
        "source/loader/ze_ldrddi.cpp",
        "source/loader/zet_ldrddi.h",
        "source/loader/zet_ldrddi.cpp",
        "source/loader/zes_ldrddi.h",
        "source/loader/zes_ldrddi.cpp",
        "source/loader/zel_tracing_ldrddi.cpp",
        "source/loader/driver_discovery.h",
    ] + select({
        "@platforms//os:windows": [
            "source/loader/windows/driver_discovery_win.cpp",
            "source/loader/windows/loader_init.cpp",
        ],
        "//conditions:default": [
            "source/loader/linux/driver_discovery_lin.cpp",
            "source/loader/linux/loader_init.cpp",
        ],
    }),
    visibility = ["//visibility:private"],
)

filegroup(
    name = "headers",
    srcs = glob([
        "include/**",
    ]),
)

cc_library(
    name = "ze_loader",
    srcs = [
        ":lib",
        ":loader",
    ] + glob(["source/inc/*.h"]),
    hdrs = [":headers"],
    copts = select({
        "@platforms//os:windows": [
            "/std:c++14",
            "/guard:cf",
            "/W3",
            "/MP",
            "/EHsc",
            "/Z7",
        ],
        "//conditions:default": [
            "-std=c++14",
            "-fpermissive",
            "-fPIC",
        ],
    }),
    defines = [
        "LOADER_VERSION_MAJOR=1",
        "LOADER_VERSION_MINOR=8",
        "LOADER_VERSION_PATCH=0",
        "L0_LOADER_VERSION=\\\"1\\\"",
        "L0_VALIDATION_LAYER_SUPPORTED_VERSION=\\\"1\\\"",
        "ZE_APIEXPORT=",
        "ZE_DLLEXPORT=",
        "ZE_STATIC_INIT",
    ],
    includes = [
        "include",
        "source/inc",
    ],
    linkopts = select({
        "@platforms//os:windows": [
            "/DEBUG",
            "/OPT:REF",
            "/OPT:ICF",
            "/CETCOMPAT",
        ] + link_libraries(["cfgmgr32.lib"]),
        "//conditions:default": ["-ldl"],
    }),
    strip_include_prefix = "include",
)

cc_library(
    name = "ze_validation_layer",
    srcs = [
        "source/layers/validation/ze_valddi.cpp",
        "source/layers/validation/ze_validation_layer.cpp",
        "source/layers/validation/ze_validation_layer.h",
        "source/layers/validation/zes_valddi.cpp",
        "source/layers/validation/zet_valddi.cpp",
    ] + glob(["source/inc/*.h"]),
    hdrs = [":headers"],
    copts = select({
        "@platforms//os:windows": [
            "/std:c++14",
            "/guard:cf",
            "/W3",
            "/MP",
            "/EHsc",
            "/Z7",
        ],
        "//conditions:default": [
            "-std=c++14",
            "-fpermissive",
            "-fPIC",
            "-fvisibility=hidden",
            "-fvisibility-inlines-hidden",
        ],
    }),
    defines = [
        "LOADER_VERSION_MAJOR=1",
        "LOADER_VERSION_MINOR=8",
        "LOADER_VERSION_PATCH=0",
        "ZE_APIEXPORT=",
        "ZE_DLLEXPORT=",
        "ZE_STATIC_INIT",
    ],
    includes = [
        "include",
        "source/inc",
        "source/layers/validation",
    ],
    linkopts = select({
        "@platforms//os:windows": [
            "/DEBUG",
            "/OPT:REF",
            "/OPT:ICF",
            "/CETCOMPAT",
        ],
        "//conditions:default": [],
    }),
    strip_include_prefix = "include",
)

cc_library(
    name = "ze_tracing_layer",
    srcs = [
        "source/layers/tracing/tracing.h",
        "source/layers/tracing/tracing_imp.cpp",
        "source/layers/tracing/tracing_imp.h",
        "source/layers/tracing/ze_tracing.cpp",
        "source/layers/tracing/ze_tracing_cb_structs.h",
        "source/layers/tracing/ze_tracing_layer.cpp",
        "source/layers/tracing/ze_tracing_layer.h",
        "source/layers/tracing/ze_tracing_register_cb.cpp",
        "source/layers/tracing/ze_trcddi.cpp",
    ] + select({
        "@platforms//os:windows": ["source/layers/tracing/windows/tracing_init.cpp"],
        "//conditions:default": ["source/layers/tracing/linux/tracing_init.cpp"],
    }) + glob(["source/inc/*.h"]),
    hdrs = [":headers"],
    copts = select({
        "@platforms//os:windows": [
            "/std:c++14",
            "/guard:cf",
            "/W3",
            "/MP",
            "/EHsc",
            "/Z7",
            "/Qspectre",
        ],
        "//conditions:default": [
            "-std=c++14",
            "-fpermissive",
            "-fPIC",
            "-fvisibility=hidden",
            "-fvisibility-inlines-hidden",
        ],
    }),
    defines = [
        "LOADER_VERSION_MAJOR=1",
        "LOADER_VERSION_MINOR=8",
        "LOADER_VERSION_PATCH=0",
        "ZE_APIEXPORT=",
        "ZE_DLLEXPORT=",
        "ZE_STATIC_INIT",
    ],
    includes = [
        "include",
        "source/inc",
        "source/layers/validation",
    ],
    linkopts = select({
        "@platforms//os:windows": [
            "/DEBUG",
            "/OPT:REF",
            "/OPT:ICF",
            "/CETCOMPAT",
        ],
        "//conditions:default": [],
    }),
    strip_include_prefix = "include",
)
