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
 * Description: benchmark rinbuffer vs deque
 ****************************************/
#include <benchmark/benchmark.h>

#include <deque>

#include "clim/ringbuffer.h"

struct Dummy {
  int x;
};

static void BM_Ringbuffer(benchmark::State& state) {
  RingBuffer<Dummy> rb(10);
  for (auto _ : state) {
    rb.Push(Dummy{});
    rb.Pop();
  }
}
BENCHMARK(BM_Ringbuffer)->Range(1, 1000000);

static void BM_deque(benchmark::State& state) {
  std::deque<Dummy> dq;
  for (auto _ : state) {
    dq.push_back(Dummy{});
    dq.pop_front();
  }
}
BENCHMARK(BM_deque)->Range(1, 1000000);
