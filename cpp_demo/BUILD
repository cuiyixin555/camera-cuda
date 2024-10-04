# load("@rules_cc//cc:defs.bzl", "cc_binary", "cc_library")

load("//bazel:msvc_helper.bzl", "link_libraries")
load("//bazel:symlink.bzl", "symlink")

cc_library(
    name = "hello-greet",
    srcs = ["hello-greet.cc"],
    hdrs = ["hello-greet.h"],
)

cc_binary(
    name = "hello-world",
    srcs = ["hello-world.cc"],
    deps = [
        ":hello-greet",
    ],
)
