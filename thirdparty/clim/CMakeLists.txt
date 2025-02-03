# Copyright (c) 2021 Intel Corporation
# Author: Wenyi Tang
# E-mail: wenyi.tang@intel.com
# Clim (C++ Lightweight Interactive Method)
# Clim is a set of header-only utitility tools

cmake_minimum_required(VERSION 3.14)

add_library(clim INTERFACE)
target_include_directories(clim INTERFACE ${CMAKE_CURRENT_SOURCE_DIR})
target_compile_features(clim INTERFACE cxx_std_17)

if(BUILD_TESTING)
  include(gtest)
  discover_all_tests(${CMAKE_CURRENT_SOURCE_DIR}/tests LINK clim FOLDER "tests")
endif()
if(TARGET benchmark AND TARGET benchmark_main)
  add_subdirectory(benchmark)
endif()
