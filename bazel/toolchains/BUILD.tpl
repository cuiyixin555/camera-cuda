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


load("@local_config_cc//:windows_cc_toolchain_config.bzl", "cc_toolchain_config")

package(default_visibility = ["//visibility:public"])

filegroup(
    name = "empty",
    srcs = [],
)

filegroup(
    name = "msvc_compiler_files",
    srcs = [],
)

cc_toolchain_suite(
    name = "toolchain",
    toolchains = {
        "x64_windows": ":cc-compiler-x64_windows",
        "x64_x86_windows": ":cc-compiler-x64_x86_windows",
        "x64_windows|msvc-cl": ":cc-compiler-x64_windows",
        "x64_x86_windows|msvc-cl": ":cc-compiler-x64_x86_windows",
    },
)

cc_toolchain(
    name = "cc-compiler-x64_windows",
    all_files = ":empty",
    ar_files = ":empty",
    as_files = ":msvc_compiler_files",
    compiler_files = ":msvc_compiler_files",
    dwp_files = ":empty",
    linker_files = ":empty",
    objcopy_files = ":empty",
    strip_files = ":empty",
    supports_param_files = 1,
    toolchain_config = ":msvc_x64",
    toolchain_identifier = "msvc_x64",
)

cc_toolchain_config(
    name = "msvc_x64",
    cpu = "x64_windows",
    compiler = "msvc-cl",
    host_system_name = "local",
    target_system_name = "local",
    target_libc = "msvcrt",
    abi_version = "local",
    abi_libc_version = "local",
    toolchain_identifier = "msvc_x64",
    msvc_env_tmp = "%{msvc_env_tmp_x64}",
    msvc_env_path = "%{msvc_env_path_x64}",
    msvc_env_include = "%{msvc_env_include_x64}",
    msvc_env_lib = "%{msvc_env_lib_x64}",
    msvc_cl_path = "%{msvc_cl_path_x64}",
    msvc_ml_path = "%{msvc_ml_path_x64}",
    msvc_link_path = "%{msvc_link_path_x64}",
    msvc_lib_path = "%{msvc_lib_path_x64}",
    cxx_builtin_include_directories = [%{msvc_cxx_builtin_include_directories_x64}],
    tool_paths = {
        "ar": "%{msvc_lib_path_x64}",
        "ml": "%{msvc_ml_path_x64}",
        "cpp": "%{msvc_cl_path_x64}",
        "gcc": "%{msvc_cl_path_x64}",
        "gcov": "wrapper/bin/msvc_nop.bat",
        "ld": "%{msvc_link_path_x64}",
        "nm": "wrapper/bin/msvc_nop.bat",
        "objcopy": "wrapper/bin/msvc_nop.bat",
        "objdump": "wrapper/bin/msvc_nop.bat",
        "strip": "wrapper/bin/msvc_nop.bat",
    },
    archiver_flags = ["/MACHINE:X64"],
    default_link_flags = ["/MACHINE:X64"],
    dbg_mode_debug_flag = "%{dbg_mode_debug_flag_x64}",
    fastbuild_mode_debug_flag = "%{fastbuild_mode_debug_flag_x64}",
)

toolchain(
    name = "cc-toolchain-x64_windows",
    exec_compatible_with = [
        "@platforms//cpu:x86_64",
        "@platforms//os:windows",
    ],
    target_compatible_with = [
        "@platforms//cpu:x86_64",
        "@platforms//os:windows",
    ],
    toolchain = ":cc-compiler-x64_windows",
    toolchain_type = "@bazel_tools//tools/cpp:toolchain_type",
)

cc_toolchain(
    name = "cc-compiler-x64_x86_windows",
    all_files = ":empty",
    ar_files = ":empty",
    as_files = ":msvc_compiler_files",
    compiler_files = ":msvc_compiler_files",
    dwp_files = ":empty",
    linker_files = ":empty",
    objcopy_files = ":empty",
    strip_files = ":empty",
    supports_param_files = 1,
    toolchain_config = ":msvc_x64_x86",
    toolchain_identifier = "msvc_x64_x86",
)

cc_toolchain_config(
    name = "msvc_x64_x86",
    cpu = "x64_windows",
    compiler = "msvc-cl",
    host_system_name = "local",
    target_system_name = "local",
    target_libc = "msvcrt",
    abi_version = "local",
    abi_libc_version = "local",
    toolchain_identifier = "msvc_x64_x86",
    msvc_env_tmp = "%{msvc_env_tmp_x86}",
    msvc_env_path = "%{msvc_env_path_x86}",
    msvc_env_include = "%{msvc_env_include_x86}",
    msvc_env_lib = "%{msvc_env_lib_x86}",
    msvc_cl_path = "%{msvc_cl_path_x86}",
    msvc_ml_path = "%{msvc_ml_path_x86}",
    msvc_link_path = "%{msvc_link_path_x86}",
    msvc_lib_path = "%{msvc_lib_path_x86}",
    cxx_builtin_include_directories = [%{msvc_cxx_builtin_include_directories_x86}],
    tool_paths = {
        "ar": "%{msvc_lib_path_x86}",
        "ml": "%{msvc_ml_path_x86}",
        "cpp": "%{msvc_cl_path_x86}",
        "gcc": "%{msvc_cl_path_x86}",
        "gcov": "wrapper/bin/msvc_nop.bat",
        "ld": "%{msvc_link_path_x86}",
        "nm": "wrapper/bin/msvc_nop.bat",
        "objcopy": "wrapper/bin/msvc_nop.bat",
        "objdump": "wrapper/bin/msvc_nop.bat",
        "strip": "wrapper/bin/msvc_nop.bat",
    },
    archiver_flags = ["/MACHINE:X86"],
    default_link_flags = ["/MACHINE:X86"],
    dbg_mode_debug_flag = "%{dbg_mode_debug_flag_x86}",
    fastbuild_mode_debug_flag = "%{fastbuild_mode_debug_flag_x86}",
)

toolchain(
    name = "cc-toolchain-x64_x86_windows",
    exec_compatible_with = [
        "@platforms//cpu:x86_64",
        "@platforms//os:windows",
    ],
    target_compatible_with = [
        "@platforms//cpu:x86_32",
        "@platforms//os:windows",
    ],
    toolchain = ":cc-compiler-x64_x86_windows",
    toolchain_type = "@bazel_tools//tools/cpp:toolchain_type",
)
