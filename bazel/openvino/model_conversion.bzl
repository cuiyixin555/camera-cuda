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


load("@local_config_csdk//:cl_compiler.bzl", "OCLOC")

def convert_model(name, xml = None, bin = None, onnx = None):
    """Convert an ONNX or OpenVINO XML model to L0 subgraph.

    L0 subgraph contains two parts: the JSON graph and the dumped kernels.
    L0 subgraph can be executed by subway::ZeSubgraphOnlineCalculator.

    Args:
        name (str): the name of the subgraph.
        xml (Label): OpenVINO IR graph file (.xml). Binary file must be also set.
        bin (Label): OpenVINO IR weight file (.bin). Set with xml.
        onnx (Label): ONNX model file (.onnx). Exclusive to xml.
    """
    if not xml and not onnx:
        fail("no model specified!")
    if xml and onnx:
        fail("xml and onnx are exclusive arguments! xml=%s, onnx=%s" % (xml, onnx))
    if xml and not bin:
        fail("when use xml, bin must be specified! xml=%s, bin=%s" % (xml, bin))

    script_name = name + ".py"
    model_files = [xml or onnx]
    if bin:
        model_files.append(bin)
    _expand_template(
        name = script_name,
        models = model_files,
        device = select({
            "//bazel/device:skl": "skl",
            "//bazel/device:tgllp": "tgllp",
            "//bazel/device:adlp": "adlp",
            "//bazel/device:mtl": "mtl",
            "//bazel/device:lnl": "lnl",
            "//bazel/device:ptl": "ptl",
            "//bazel/device:nvl": "nvl",
            "//bazel/device:sginle": "single",
        }),
    )
    native.py_binary(
        name = name,
        srcs = [script_name],
        data = ["//subway/tools:ov_graph_dumper", "//subway/tools:ov_graph_check"],
        deps = ["//subway/python/graph:dump_graph"],
        env = {
            "SUBWAY_KERNEL_DUMP": "1",
            "SUBWAY_LOG": "4",
        },
    )

def _expand_template_impl(ctx):
    """Inject targets of interest into refresh.template.py, and set it up to be run."""
    script = ctx.actions.declare_file(ctx.attr.name)
    ctx.actions.expand_template(
        output = script,
        is_executable = True,
        template = ctx.file._script_template,
        substitutions = {
            "{OCLOC}": ctx.attr._compiler,
            "{MODEL}": "%s" % [f.path for m in ctx.attr.models for f in m.files.to_list()],
            "{NAME}": ctx.label.name,
            "{DEVICE}": ctx.attr.device,
        },
    )
    runfiles = ctx.runfiles(files = [f for m in ctx.attr.models for f in m.files.to_list()])
    return DefaultInfo(files = depset([script]), runfiles = runfiles)

_expand_template = rule(
    attrs = {
        "models": attr.label_list(
            allow_empty = False,
            allow_files = True,
            mandatory = True,
            doc = "specify model files, can be .onnx and .xml/.bin",
        ),
        "device": attr.string(default = "adlp", doc = "specify target device platform"),
        "_script_template": attr.label(allow_single_file = True, default = "@//bazel/openvino:py_template"),
        "_compiler": attr.string(default = OCLOC),
    },
    implementation = _expand_template_impl,
)
