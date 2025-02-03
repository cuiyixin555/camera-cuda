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

"""
This file is some useful compile options for bazel binary and test
"""

RAP_MSVC_COPTS = [
    # Enables standard C++ stack unwinding. Catches only standard C++ exceptions
    # when you use catch(...) syntax. Unless /EHc is also specified, the
    # compiler assumes that functions declared as extern "C" may throw a C++
    # exception.
    "/EHsc",
    "/GR",  # Enable run-time type information (RTTI)
    # Specify standards conformance mode to the compiler. Use this option to help
    # you identify and fix conformance issues in your code, to make it both more
    # correct and more portable.
    "/permissive-",
    "/D_WIN32_WINNT=0xA00",  # overwrite built-in 0x0601 which is old win7
    # These are security flags required by Intel policy
    "/sdl",
    "/GS",
    "/ZH:SHA_256",
    "/guard:cf",
    "/Qspectre",
    # Disable noisy warnings
    "/wd4324",  # struct is aligned
    "/wd4819",  # non-unicode charactors
]

RAP_CLANG_COPTS = [
    "-Wno-macro-redefined",
    "-D_WIN32_WINNT=0xA00",  # overwrite built-in 0x0601 which is old win7
    "-Wno-comment",
    "-Wno-inconsistent-missing-override",
    "-Wno-switch",
    "-Wno-unused-but-set-variable",
    "-Wno-unused-command-line-argument",
    "-Wno-unused-function",
    "-Wno-unused-value",
    "-Wno-unused-variable",
    "-Wno-unknown-warning-option",
]

RAP_MSVC_LINKOPTS = [
    "/DEBUG:FULL",
    "/guard:cf",
    "/DYNAMICBASE",
]

RAP_DEFAULT_COPTS = select({
    "@platforms//os:windows": RAP_MSVC_COPTS + [
        "/std:c++20",
        # Set warning level to very strict and treat them as errors
        "/W4",
        "/WX",
        # Use wchar_t to support unicode.
        # The Windows API will use *W version under this flag (On the opposite
        # they are *A version).
        "/D_UNICODE",
        "/DUNICODE",
        # /Z7 compiles debug information to object file (.obj), see
        # https://docs.microsoft.com/en-us/cpp/build/reference/z7-zi-zi-debug-information-format
        # /Zi would lead to an known bug of cl.exe, unless we compile bazel
        # with only one thread (-j=1). So we use /Z7 here.
        "/Z7",
    ],
    "//conditions:default": [
        "-std=c++20"
    ],
    "@platforms//os:linux": RAP_CLANG_COPTS + [
        "-std=c++20",
        "-Wall",
        "-Wextra",
        "-Werror",
    ]
})
