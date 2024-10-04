"""
Copyright (c) 2022 Intel Corporation
Author: Wenyi Tang
E-mail: wenyi.tang@intel.com
bazel BUILD for DirectXTex
"""

package(default_visibility = ["//visibility:public"])

licenses(["notice"])

exports_files(["LICENSE"])

cc_library(
    name = "DirectXTex",
    srcs = [
        "DirectXTex/BC.cpp",
        "DirectXTex/BC.h",
        "DirectXTex/BC4BC5.cpp",
        "DirectXTex/BC6HBC7.cpp",
        "DirectXTex/DDS.h",
        "DirectXTex/DirectXTexCompress.cpp",
        "DirectXTex/DirectXTexConvert.cpp",
        "DirectXTex/DirectXTexDDS.cpp",
        "DirectXTex/DirectXTexHDR.cpp",
        "DirectXTex/DirectXTexImage.cpp",
        "DirectXTex/DirectXTexMipmaps.cpp",
        "DirectXTex/DirectXTexMisc.cpp",
        "DirectXTex/DirectXTexNormalMaps.cpp",
        "DirectXTex/DirectXTexP.h",
        "DirectXTex/DirectXTexPMAlpha.cpp",
        "DirectXTex/DirectXTexResize.cpp",
        "DirectXTex/DirectXTexTGA.cpp",
        "DirectXTex/DirectXTexUtil.cpp",
        "DirectXTex/filters.h",
        "DirectXTex/scoped.h",
    ] + select({
        "@platforms//os:windows": [
            "DirectXTex/DirectXTexFlipRotate.cpp",
            "DirectXTex/DirectXTexWIC.cpp",
            "DirectXTex/d3dx12.h",  # BUILD_DX12
            "DirectXTex/DirectXTexD3D12.cpp",  # BUILD_DX12
        ],
        "//conditions:default": [],
    }),
    hdrs = [
        "DirectXTex/DirectXTex.h",
        "DirectXTex/DirectXTex.inl",
    ],
    copts = select({
        "@subway//subway:windows-clang-cl": [
            "-Wpedantic",
            "-Wextra",
        ],
        "@platforms//os:windows": [
            "/Wall",
            "/GR-",
            "/guard:cf",
            "/sdl",
            "/permissive-",
            "/Zc:__cplusplus",
            "/ZH:SHA_256",
            "/Zc:preprocessor",
            "/Qspectre",
            "/wd5105",
        ],
        "//conditions:default": [],
    }),
    includes = ["DirectXTex"],
    linkopts = select({
        "@platforms//os:windows": [
            "/DYNAMICBASE",
            "/NXCOMPAT",
            "/CETCOMPAT",
        ],
        "//conditions:default": [],
    }),
    local_defines = [
        "_UNICODE",
        "UNICODE",
        "_WIN32_WINNT=0x0A00",
    ],
    strip_include_prefix = "DirectXTex",
    target_compatible_with = ["@platforms//os:windows"],
)
