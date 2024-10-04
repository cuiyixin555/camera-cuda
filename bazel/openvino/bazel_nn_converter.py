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


import argparse
import logging
import os
import shutil
import subprocess as sp
import tempfile
from pathlib import Path

# relative path in bazel output directory
from subway.subway.python.graph.dump_graph import DumpGraph

DUMPER = "subway/tools/ov_graph_dumper"
CHECKER = "subway/tools/ov_graph_check"
if os.name == 'nt':
    DUMPER += ".exe"
    CHECKER += ".exe"

NAME = "{NAME}"
OCLOC = "{OCLOC}"
MODEL_LIST = {MODEL}
DEVICE = "{DEVICE}"

logger = logging.getLogger('subway')


def copy_kernels(kernels_cache, out_dir, spv: bool):
    """Test and copy kernels to destination.

    Dumped kernels are not all mandatory because some of them are used only in optimization phase.
    Discard non-mandatory kernels.

    Args:
        kernels_cache (str): directory of kernels
        out_dir (str): destination folder
        spv (bool): kernels in SPIR-V format
    """
    # copy kernels
    dg = DumpGraph(graph_file='intel_gpu_kernel_dump.json', use_base64=True)
    input_shapes = dg.dump(out_dir / Path(NAME).with_suffix('.json'))

    subgraph = out_dir / Path(NAME).with_suffix('.json')
    for kernel in kernels_cache.glob('*.' + f"{'spv' if spv else 'cl_cache'}"):
        # rename each kernel to .bak and test if it's mandatory to run
        os.rename(kernel, kernel.with_suffix('.bak'))
        kernel_bak = kernel.with_suffix('.bak')
        logger.info(f"check {kernel.name}")
        ret = sp.run(
            map(str, [' ', subgraph, kernels_cache, input_shapes]),
            executable=CHECKER, stdout=sp.PIPE, stderr=sp.PIPE)
        if ret.returncode == 0:
            # If it's not, remove the kernel
            os.remove(kernel_bak)
        else:
            print(ret.stdout.decode())
            # copy only mandatory kernels to output path
            os.rename(kernel_bak, kernel)
            logger.info(f"{kernel.name} is mandatory, copy {kernel.name} to {out_dir}")
            shutil.copyfile(kernel, out_dir / kernel.name)


def compile(ocloc, work_dir, device, disable_ze_bin, ze_bin, spv):
    """Recompile sources to binaries.

    Args:
        ocloc (str): path to opencl compiler
        work_dir (str): working directory
        device (str): target device
        disable_ze_bin (bool): disable ze-bin, ze-bin allows fast binary creation cross driver versions.
        ze_bin (bool): enable ze-bin, ze-bin allows fast binary creation cross driver versions.
        spv (bool): enable SPIR-V, compile to SPIR-V rather than binary.

    Raises:
        RuntimeError: compile failed

    Returns:
        str: directory of compiled files
    """
    # cross compile kernels
    cldnn_kernels = Path(work_dir) / 'cldnn_ov/cldnn_kernels_cc'
    cldnn_sources = Path(work_dir) / 'cldnn_ov/cldnn_sources'
    if cldnn_kernels.exists():
        for i in cldnn_kernels.rglob('*'):
            os.remove(i)
    cldnn_kernels.mkdir(exist_ok=True, parents=True)
    cmd = f"{ocloc.resolve()} -file {{}} -device {device} -out_dir {cldnn_kernels} -output_no_suffix -options \"-w\""
    if disable_ze_bin:
        # use --format patchtokens to disable ze-bin
        cmd += f" --format patchtokens"
    elif ze_bin:
        # use --format zebin to enable ze-bin
        cmd += f" --format zebin"
    else:
        # use default format
        pass
    for source in cldnn_sources.glob('*.cl'):
        retcode = sp.call(cmd.format(source), shell=True)
        if retcode == 0:
            if os.path.exists(cldnn_kernels / source.with_suffix('.gen').name):
                # .gen file is not generated in ver 2022WW42
                os.remove(cldnn_kernels / source.with_suffix('.gen').name)
            if spv:
                os.remove(cldnn_kernels / (source.stem + '.bin'))
                #os.remove(cldnn_kernels / source.stem)
            else:
                os.rename(cldnn_kernels / (source.stem + '.bin'), cldnn_kernels / source.with_suffix('.cl_cache').name)
                #os.rename(cldnn_kernels / source.stem, cldnn_kernels / source.with_suffix('.cl_cache').name)
                os.remove(cldnn_kernels / source.with_suffix('.spv').name)
        else:
            raise RuntimeError(f"ocloc compile error for:\n {cmd.format(source)}")
    return cldnn_kernels


def main(device, disable_ze_bin, ze_bin, spv, out_dir, keep_temp):
    """Main entry of the conversion.

    - Call dump program to dump kernels and graph
    - Convert to subgraph
    - Compile kernels and copy to destination

    Args:
        device (str): target device
        disable_ze_bin (bool): disable ze-bin flag
        ze_bin (bool): enable ze-bin flag
        spv (bool): enable spir-v flag
        out_dir (str): output directory
        keep_temp (bool): do not delete temp files

    Raises:
        FileNotFoundError: MODEL_LIST from template is invalid
        RuntimeError: Dump failed
    """
    if len(MODEL_LIST) == 2:
        # OpenVINO IR
        model_xml, model_bin = map(Path, MODEL_LIST)
    else:
        # ONNX
        model_xml = Path(MODEL_LIST[0])
        model_bin = model_xml
    if not model_xml.exists() or not model_bin.exists():
        raise FileNotFoundError(f"Model not found: {model_xml} or {model_bin}")

    out_dir.mkdir(exist_ok=True, parents=True)
    tmpdir = tempfile.mkdtemp()
    work_dir = Path(tmpdir) / "ov_dump"
    logger.info("Dumping OpenVINO graph...")
    ret = sp.run(
        map(str, [' ', model_xml, model_bin, work_dir]),
        executable=DUMPER, stdout=sp.PIPE, stderr=sp.PIPE)
    if ret.returncode != 0:
        raise RuntimeError(ret.stdout.decode())
    logger.info("Compiling sources...")
    kernels_cache = compile(Path(OCLOC), work_dir, device, disable_ze_bin, ze_bin, spv)
    logger.info("Copying kernels...")
    copy_kernels(kernels_cache, out_dir, spv)
    logger.info("Congradulations, conversion done!!")
    if not keep_temp:
        shutil.rmtree(tmpdir)
    else:
        logger.info(f"Temporary files are kept at {tmpdir}")


if __name__ == '__main__':
    parser = argparse.ArgumentParser(description="""Bazel NN model Converter

Add arguments after `--` to override the default values.

'ze-bin' feature is enabled by default on latest compute SDK, while it is not
enabled by default on legacy compute SDK.

Example:
    >>> bazel run //data/model:srcnn
    >>> bazel run //data/model:srcnn -- --disable_ze_bin --device mtl
    >>> bazel run //data/model:srcnn -- --ze_bin --device mtl
""")
    parser.add_argument("--device",
                        choices=('skl', 'tgllp', 'adlp', 'mtl', 'lnl','ptl','nvl'),
                        default=DEVICE,
                        help="""specify the target device, binary kernel
compiled at that device platform. Cross-device will trigger recompile when
loading kernels""")
    parser.add_argument("--disable_ze_bin",
                        action='store_true',
                        help="disable ze-bin feature")
    parser.add_argument("--ze_bin",
                        action='store_true',
                        help="enable ze-bin feature")
    parser.add_argument("--spv",
                        action='store_true',
                        help="compile to SPIR-V rather than binary")
    parser.add_argument("--out-dir", type=Path, default=Path.cwd())
    parser.add_argument("--keep-temp-files", action='store_true')
    args = parser.parse_args()

    logging.basicConfig(level=logging.INFO)
    print("cwd=", Path.cwd().resolve())
    print("ocloc=", Path(OCLOC).resolve())
    print("out_dir=", args.out_dir.resolve())
    main(args.device, args.disable_ze_bin, args.ze_bin, args.spv, args.out_dir, args.keep_temp_files)
