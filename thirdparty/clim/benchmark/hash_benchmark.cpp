/*
 * INTEL CONFIDENTIAL
 *
 * Copyright (C) 2021-2023 Intel Corporation
 *
 * This software and the related documents are Intel copyrighted materials,
 * and your use of them is governed by the express license under which they
 * were provided to you ("License"). Unless the License provides otherwise,
 * you may not use, modify, copy, publish, distribute, disclose or transmit
 * this software or the related documents without Intel's prior written
 * permission. This software and the related documents are provided as is, with
 * no express or implied warranties, other than those that are expressly stated
 * in the License.
 */
/****************************************
 * Description: bench hashers
 ****************************************/
#include <benchmark/benchmark.h>

#include <string>
#include <utility>

#include "clim/hasher/hash.h"

static std::string MakeString(size_t len) { return std::string(len, 'x'); }

static void BM_std_hash(benchmark::State& state) {
  std::string str = MakeString(state.range(0));
  auto hasher = std::hash<std::string>();
  for (auto _ : state) {
    size_t hash = hasher(str);
    benchmark::DoNotOptimize(hash);
  }
}
BENCHMARK(BM_std_hash)->RangeMultiplier(2)->Range(1, 4096);

static void BM_city_hash(benchmark::State& state) {
  std::string str = MakeString(state.range(0));
  auto hasher = city::Hash<std::string>();
  for (auto _ : state) {
    size_t hash = hasher(str);
    benchmark::DoNotOptimize(hash);
  }
}
BENCHMARK(BM_city_hash)->RangeMultiplier(2)->Range(1, 4096);

static void BM_murmur3_hash(benchmark::State& state) {
  std::string str = MakeString(state.range(0));
  auto hasher = murmur3::Hash<std::string>();
  for (auto _ : state) {
    size_t hash = hasher(str);
    benchmark::DoNotOptimize(hash);
  }
}
BENCHMARK(BM_murmur3_hash)->RangeMultiplier(2)->Range(1, 4096);

static void BM_mix_hash(benchmark::State& state) {
  std::string str = MakeString(state.range(0));
  auto hasher = MixStringHash();
  for (auto _ : state) {
    size_t hash = hasher(str);
    benchmark::DoNotOptimize(hash);
  }
}
BENCHMARK(BM_mix_hash)->RangeMultiplier(2)->Range(1, 4096);
