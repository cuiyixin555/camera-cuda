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


# From https://bazel.build/versions/master/docs/be/c-cpp.html#cc_library.srcs
_SOURCE_EXTENSIONS = [source_ext for source_ext in """
.c
.cc
.cpp
.h
.hpp
.inc
""".split("\n") if len(source_ext)]

# The cpplint.py command-line argument so it doesn't skip our files!
_EXTENSIONS_ARGS = ["--extensions=" + ",".join(
    [ext[1:] for ext in _SOURCE_EXTENSIONS],
)]

def _extract_labels(srcs):
    """Convert a srcs= or hdrs= value to its set of labels."""

    # Tuples are already labels.
    if type(srcs) == type(()):
        return list(srcs)
    return []

def _is_source_label(label):
    for extension in _SOURCE_EXTENSIONS:
        if label.endswith(extension):
            return True
    return False

def _is_excluded_label(label, exclude_files):
    for file in exclude_files:
        if label.endswith(file):
            return True
    return False

def _add_linter_rules(name, source_labels, source_filenames):
    # Common attributes for all of our py_test invocations.
    size = "small"
    tags = ["cpplint"]

    # Google cpplint.
    cpplint_cfg = ["//:CPPLINT.cfg"] + native.glob(["CPPLINT.cfg"])
    native.py_test(
        name = name + "_cpplint",
        srcs = ["@cpplint//:cpplint"],
        data = cpplint_cfg + source_labels,
        args = _EXTENSIONS_ARGS + source_filenames,
        main = "cpplint.py",
        size = size,
        tags = tags,
    )

def cpplint(exclude_files = []):
    """For every rule in the BUILD file, adds a test rule to run cpplint over the C++ sources listed in that rule.

    Thus, BUILD file authors should call this function at the *end* of every C++-related BUILD file.
    By default, only the CPPLINT.cfg from the project root and the current directory are used.

    Args:
        exclude_files: add file list to be excluded from cpplint
    """

    # Iterate over all rules.
    for rule in native.existing_rules().values():
        # Extract the list of C++ source code labels and convert to filenames.
        candidate_labels = (
            _extract_labels(rule.get("srcs", ())) +
            _extract_labels(rule.get("hdrs", ()))
        )
        source_labels = [
            label
            for label in candidate_labels
            if _is_source_label(label) and not _is_excluded_label(label, exclude_files)
        ]
        source_filenames = ["$(location %s)" % x for x in source_labels]

        # Run the cpplint checker as a unit test.
        if len(source_filenames) > 0:
            _add_linter_rules(rule["name"], source_labels, source_filenames)
