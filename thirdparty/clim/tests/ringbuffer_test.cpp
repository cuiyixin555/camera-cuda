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
 * Description:
 ****************************************/
#include "clim/ringbuffer.h"

#include <gtest/gtest.h>

TEST(Ringbuffer, EmptyCheck) {
  RingBuffer<int> rb;
  ASSERT_EQ(rb.Capacity(), 2);
  EXPECT_TRUE(rb.Empty());
  rb.Push(1);
  EXPECT_FALSE(rb.Empty());
  rb.Push(2);
  rb.Push(3);
  rb.Pop();
  rb.Pop();
  EXPECT_TRUE(rb.Empty());
}

TEST(Ringbuffer, Fifo) {
  RingBuffer<int> rb;
  rb.Push(1);
  rb.Push(2);
  EXPECT_EQ(rb.Pop(), 1);
  EXPECT_EQ(rb.Pop(), 2);
  rb.Push(1);
  rb.Push(2);
  rb.Push(3);
  EXPECT_EQ(rb.Pop(), 2);
  EXPECT_EQ(rb.Pop(), 3);
  EXPECT_TRUE(rb.Empty());
}

TEST(Ringbuffer, BigFifo) {
  RingBuffer<int> rb(3);
  rb.Push(1);
  rb.Push(2);
  EXPECT_EQ(rb.Pop(), 1);
  EXPECT_EQ(rb.Pop(), 2);
  rb.Push(1);
  rb.Push(2);
  rb.Push(3);
  rb.Push(4);
  EXPECT_EQ(rb.Pop(), 2);
  EXPECT_EQ(rb.Pop(), 3);
  EXPECT_EQ(rb.Pop(), 4);
  EXPECT_TRUE(rb.Empty());
  EXPECT_EQ(rb.Capacity(), 3);
}
