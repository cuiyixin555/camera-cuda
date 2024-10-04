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

def _depset_to_list(x):
    """Helper function to convert depset to list."""
    iter_list = x.to_list() if type(x) == "depset" else x
    return iter_list

def _symlink(ctx):
    out_files = []
    for t in ctx.attr.targets:
        for file in _depset_to_list(t.files):
            if ctx.attr.keep_source_tree:
                out = ctx.actions.declare_file(file.dirname + "/" + file.basename)
            else:
                out = ctx.actions.declare_file(file.basename)
            ctx.actions.symlink(output = out, target_file = file)
            out_files.append(out)
    return DefaultInfo(data_runfiles = ctx.runfiles(files = out_files))

symlink = rule(
    implementation = _symlink,
    attrs = {
        "targets": attr.label_list(mandatory = True, doc = "binary targets to make the symlink"),
        "keep_source_tree": attr.bool(default = False, doc = "keep the source directory tree"),
    },
)
