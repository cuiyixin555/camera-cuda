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

workspace(name = "camera_cuda")

## If you want to disable the access to internet, replace below call to local_repository
# load("@bazel_tools//tools/build_defs/repo:http.bzl", "http_archive")
load("@bazel_tools//tools/build_defs/repo:git.bzl", "new_git_repository")

# This is used to select all contents of the archives for CMake-based packages to give CMake access to them.
top = """
filegroup(name = "cmakelists", srcs = glob(["CMakeLists.txt"]), visibility = ["//visibility:public"])
"""

repo_deps = "../repo-deps"

# local repo rules_cc to avoid bazel build_tools downloading rules_cc from remote server.
local_repository(
    name = "rules_cc",
    path = repo_deps + "/bazelbuild/rules_cc",
)

local_repository(
    name = "rules_java",
    path = repo_deps + "/bazelbuild/rules_java",
)

local_repository(
    name = "rules_python",
    path = repo_deps + "/bazelbuild/rules_python",
)

local_repository(
    name = "rules_proto",
    path = repo_deps + "/bazelbuild/rules_proto",
)

# https://github.com/bazelbuild/bazel-skylib/releases
local_repository(
    name = "bazel_skylib",
    path = repo_deps + "/bazelbuild/bazel-skylib",
)

load("@bazel_skylib//:workspace.bzl", "bazel_skylib_workspace")

bazel_skylib_workspace()

new_local_repository(
    name = "repo_deps",
    build_file_content = """
filegroup(name = "top", srcs = ["LICENSE"], visibility = ["//visibility:public"])
""",
    path = repo_deps,
)

load("@camera_cuda//bazel:local_archive.bzl", "local_archive")

# extract local prebuilt cmake tools
local_archive(
    name = "cmake_win32",
    src = "@camera_deps//:build_tools/windows/cmake-3.21.5-windows-x86_64.zip",
    build_file_content = """
filegroup(name = "cmake", srcs = glob(["bin/cmake.exe"]), visibility = ["//visibility:public"])
""",
    strip_prefix = "cmake-3.21.5-windows-x86_64",
)

local_archive(
    name = "cmake_default",
    src = "@camera_deps//:build_tools/linux/cmake-3.21.6-linux-x86_64.tar.gz",
    build_file_content = """
filegroup(name = "cmake", srcs = glob(["bin/cmake"]), visibility = ["//visibility:public"])
""",
    strip_prefix = "cmake-3.21.6-linux-x86_64",
)

new_local_repository(
    name = "ninja_win32",
    build_file_content = """
filegroup(name = "ninja", srcs = ["ninja.exe"], visibility = ["//visibility:public"])
""",
    path = repo_deps + "/build_tools/windows/ninja",
)

new_local_repository(
    name = "ninja_default",
    build_file_content = """
filegroup(name = "bin", srcs = ["ninja"], visibility = ["//visibility:public"])
""",
    path = repo_deps + "/build_tools/linux",
)

new_local_repository(
    name = "cpplint",
    build_file = "@//bazel:cpplint.BUILD",
    path = repo_deps + "/cpplint",
)

new_local_repository(
    name = "cl_runtime",
    build_file = "@//bazel:cl.BUILD",
    path = repo_deps + "/win32/cl_runtime",
)

new_local_repository(
    name = "clDNNx",
    build_file_content = top,
    path = repo_deps + "/clDNNx",
)

local_repository(
    name = "com_github_google_flatbuffers",
    path = repo_deps + "/flatbuffers",
)

new_local_repository(
    name = "fmt",
    build_file = "@//bazel:fmt.BUILD",
    path = repo_deps + "/fmt",
)

local_repository(
    name = "gtest",
    path = repo_deps + "/gtest",
)

new_local_repository(
    name = "jsoncpp",
    build_file = "@//bazel:jsoncpp.BUILD",
    path = repo_deps + "/jsoncpp",
)

new_local_repository(
    name = "opencv",
    build_file_content = top,
    path = repo_deps + "/opencv_mini",
)

new_local_repository(
    name = "pvl",
    build_file_content = top,
    path = repo_deps + "/pvl",
)

new_local_repository(
    name = "spdlog",
    build_file = "@//bazel:spdlog.BUILD",
    path = repo_deps + "/spdlog",
)

new_local_repository(
    name = "ze_loader",
    build_file = "@//bazel:ze_loader.BUILD",
    path = repo_deps + "/ze_loader",
)

new_local_repository(
    name = "linux_ze_loader",
    build_file = "@//bazel:linux_ze_loader.BUILD",
    path = repo_deps + "/linux/ze_loader",
)

local_repository(
    name = "clim",
    path = "thirdparty/clim",  # codespell:ignore thirdparty
    repo_mapping = {
        "@com_google_googletest": "@gtest",
        "@com_google_benchmark": "@benchmark",
    },
)

new_local_repository(
    name = "latest_compute_sdk_win32",
    build_file_content = """
filegroup(name = "top", srcs = ["COPYING"], visibility = ["//visibility:public"])
filegroup(name = "compiler", srcs = glob(["compiler/**"]), visibility = ["//visibility:public"])
filegroup(name = "cm_api", srcs = glob(["cmemu/include/**/*.h"]), visibility = ["//visibility:public"])
""",
    path = repo_deps + "/win32/latest_compute_sdk",
)

new_local_repository(
    name = "legacy_compute_sdk_win32",
    build_file_content = """
filegroup(name = "top", srcs = ["COPYING"], visibility = ["//visibility:public"])
filegroup(name = "compiler", srcs = glob(["compiler/**"]), visibility = ["//visibility:public"])
filegroup(name = "cm_api", srcs = glob(["cmemu/include/**/*.h"]), visibility = ["//visibility:public"])
""",
    path = repo_deps + "/win32/legacy_compute_sdk",
)

new_local_repository(
    name = "openvino_intel_gpu_plugin_dump",
    build_file = "@//bazel/openvino:openvino_intel_gpu_plugin_dump.BUILD",
    path = repo_deps + "/openvino_intel_gpu_plugin",
)

new_local_repository(
    name = "openvino_2022_win32",
    build_file = "@//bazel/openvino:openvino_2022_win32.BUILD",
    path = repo_deps + "/win32/openvino_2022",
)

new_local_repository(
    name = "openvino_vpu_win32",
    build_file = "@//bazel/openvino:openvino_vpu_static_win32.BUILD",
    path = repo_deps + "/win32/openvino_vpu_static",
)

new_local_repository(
    name = "d3dx12",
    build_file_content = """
cc_library(name = "d3dx12", hdrs = ["d3dx12.h"], visibility = ["//visibility:public"])
""",
    path = "thirdparty/directx",  # codespell:ignore thirdparty
)

new_local_repository(
    name = "DirectXTK12",
    build_file_content = top,
    path = repo_deps + "/DirectXTK12",
)

new_local_repository(
    name = "DirectXTex",
    build_file = "@//bazel:directxtex.BUILD",
    path = repo_deps + "/DirectXTex",
)

new_local_repository(
    name = "imgui",
    build_file = "@//bazel:imgui.BUILD",
    path = repo_deps + "/imgui",
)

local_repository(
    name = "pybind11_bazel",
    path = repo_deps + "/pybind11_bazel",
)

# Point to the commit that deprecates the usage of Eigen::MappedSparseMatrix.
new_local_repository(
    name = "pybind11",
    build_file = "@pybind11_bazel//:pybind11.BUILD",
    path = repo_deps + "/pybind11",
)

load("@pybind11_bazel//:python_configure.bzl", "python_configure")

python_configure(name = "local_config_python")

# Configure local dev

# # https://github.com/bazelbuild/rules_foreign_cc/releases
local_repository(
    name = "rules_foreign_cc",
    path = repo_deps + "/bazelbuild/rules_foreign_cc",
)

load("@rules_foreign_cc//foreign_cc:repositories.bzl", "rules_foreign_cc_dependencies")

# This sets up some common toolchains for building targets. For more details, please see
# https://bazelbuild.github.io/rules_foreign_cc/0.7.1/flatten.html#rules_foreign_cc_dependencies
rules_foreign_cc_dependencies(
    cmake_version = "3.21.5",
    native_tools_toolchains = [
        "@//cmake/toolchain:repo_deps_cmake_win32_toolchain",
        "@//cmake/toolchain:repo_deps_ninja_win32_toolchain",
        "@//cmake/toolchain:repo_deps_cmake_toolchain",
        "@//cmake/toolchain:repo_deps_ninja_toolchain",
    ],
    register_built_tools = False,
    register_default_tools = False,
    register_preinstalled_tools = True,
)




