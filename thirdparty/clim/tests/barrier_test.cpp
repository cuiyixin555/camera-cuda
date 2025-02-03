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
 * Description: Test barrier object
 ****************************************/
#include "clim/barrier.h"

#include <gtest/gtest.h>

#include <thread>

TEST(Barrier, SingleThread) {
  Barrier b(1);
  EXPECT_FALSE(b.Notified());
  b.Notify();
  b.Wait();
  EXPECT_FALSE(b.Notified());

  Barrier bb(2);
  EXPECT_FALSE(bb.Notified());
  EXPECT_FALSE(bb.WaitFor(std::chrono::seconds(0)));
  EXPECT_FALSE(bb.Notified());
  bb.Notify();
  bb.Notify();
  EXPECT_TRUE(bb.WaitFor(std::chrono::seconds(0)));
  EXPECT_TRUE(bb.Notified());
}

TEST(Barrier, TwinThreads) {
  Barrier b(1);

  std::thread waiter([&]() { b.Wait(); });
  std::thread publisher([&]() {
    std::this_thread::sleep_for(std::chrono::milliseconds(100));
    b.Notify();
  });

  waiter.join();
  publisher.join();
  b.Wait();
  EXPECT_TRUE(b.Notified());
}
