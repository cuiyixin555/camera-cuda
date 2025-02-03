# Copyright (c) 2022 Intel Corporation
# Author: Wenyi Tang
# E-mail: wenyi.tang@intel.com

add_executable(hash_benchmark hash_benchmark.cpp)
target_link_libraries(hash_benchmark PRIVATE clim benchmark benchmark_main)

add_executable(stringview_hashmap_benchmark stringview_hashmap_benchmark.cpp)
target_link_libraries(stringview_hashmap_benchmark PRIVATE clim benchmark benchmark_main)

add_executable(string_split_benchmark string_split_benchmark.cpp)
target_link_libraries(string_split_benchmark PRIVATE clim benchmark benchmark_main)
