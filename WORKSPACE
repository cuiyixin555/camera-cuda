# MIT License

# Copyright (c) 2024-2025 Cui, Xin

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

load("@bazel_tools//tools/build_defs/repo:git.bzl", "new_git_repository")
load("@bazel_tools//tools/build_defs/repo:http.bzl", "http_archive")

# # using the local bazel rules
camera_supp = "../camera-supp"

# Google test
local_repository(
    name = "gtest",
    path = camera_supp + "/gtest",
)

# Bazel rules of c/c++
local_repository(
    name = "rules_cc",
    path = camera_supp + "/bazelbuild/rules_cc",
)

# # https://github.com/bazelbuild/rules_foreign_cc/releases
local_repository(
    name = "rules_foreign_cc",
    path = camera_supp + "/bazelbuild/rules_foreign_cc",
)

load("@rules_foreign_cc//foreign_cc:repositories.bzl", "rules_foreign_cc_dependencies")

# Bazel rules of java
local_repository(
    name = "rules_java",
    path = camera_supp + "/bazelbuild/rules_java",
)

local_repository(
    name = "rules_python",
    path = camera_supp + "/bazelbuild/rules_python",
)

local_repository(
    name = "rules_proto",
    path = camera_supp + "/bazelbuild/rules_proto",
)

local_repository(
    name = "bazel_skylib",
    path = camera_supp + "/bazelbuild/bazel-skylib",
)

new_local_repository(
    name = "level_zero",
    build_file = "@//bazel:level_zero.BUILD",
    path = camera_supp + "/ze_loader",
)

new_local_repository(
    name = "spdlog",
    build_file = "@//bazel:spdlog.BUILD",
    path = camera_supp + "/spdlog",
)

new_local_repository(
    name = "fmt",
    build_file = "@//bazel:fmt.BUILD",
    path = camera_supp + "/fmt",
)

# using new_local_repository for defining opencv path
new_local_repository(
    name = "opencv",
    build_file = "@//bazel:opencv.BUILD",
    path = camera_supp + "/opencv/build",
)

local_repository(
    name = "clim",
    path = "thirdparty/clim",  # codespell:ignore thirdparty
    repo_mapping = {
        "@com_google_googletest": "@gtest",
        "@com_google_benchmark": "@benchmark",
    },
)

# Bazel rules of cuda
local_repository(
    name = "rules_cuda",
    path = camera_supp + "/bazelbuild/rules_cuda",
)

load("@rules_cuda//cuda:repositories.bzl", "register_detected_cuda_toolchains", "rules_cuda_dependencies")

rules_cuda_dependencies()

register_detected_cuda_toolchains()

load("@bazel_skylib//:workspace.bzl", "bazel_skylib_workspace")

bazel_skylib_workspace()

new_local_repository(
    name = "camera_supp",
    build_file_content = """
filegroup(name = "top", srcs = ["LICENSE"], visibility = ["//visibility:public"])
""",
    path = camera_supp,
)

