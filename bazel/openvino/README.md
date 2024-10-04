# Custom openvino intel gpu plugin
1. `git clone https://github.com/openvinotoolkit/openvino -b 2022.1.0`
2. `git apply kernel_dump.patch`
3. run `python src/plugins/intel_gpu/src/kernel_selector/core/common/primitive_db_gen.py -kernels src/plugins/intel_gpu/src/kernel_selector/core/cl_kernels -out_path src/plugins/intel_gpu/include/codegen/include -out_file_name_prim_db "ks_primitive_db.inc" -out_file_name_batch_headers "ks_primitive_db_batch_headers.inc"` manually.

The above two steps have already been taken in `camera-deps`, repeat them if you want to upgrade openvino to a new version.

A test sample is written in `tests/openvino_test`, run `bazel test //tests/openvino_test:openvino_gpu_dump_test` to check.

**NOTE: CMAKE build is not supported!**
