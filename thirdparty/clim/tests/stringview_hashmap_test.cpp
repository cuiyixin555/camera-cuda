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
 * Description: unit test of StringviewHashmap
 ****************************************/
#include <gtest/gtest.h>

#include "clim/stringview_hash_map.h"

TEST(Hashmap, EmplaceNewValue) {
  StringviewHashmap<int> ht;
  ht.try_emplace("a", 0);
  ht.try_emplace("b", 1);
  ht.try_emplace("c", 2);
  ht.try_emplace("d", 3);
  std::string e("e");
  ht.try_emplace(e, 4);
  EXPECT_EQ(ht.find("a")->second, 0);
  EXPECT_EQ(ht.find("b")->second, 1);
  EXPECT_EQ(ht.find("c")->second, 2);
  EXPECT_EQ(ht.find("d")->second, 3);
  EXPECT_EQ(ht.find("e")->second, 4);
  EXPECT_EQ(ht.find("A"), ht.end());
  // the key holds on hashmap shouldn't depend on callers.
  EXPECT_NE(ht.find("e")->first.data(), e.data());
}

TEST(Hashmap, EraseValue) {
  StringviewHashmap<int> ht;
  ht["a"] = 0;
  ht["b"] = 1;
  ht.erase("b");
  EXPECT_EQ(ht.count("a"), 1);
  EXPECT_EQ(ht.count("b"), 0);
  EXPECT_EQ(ht.count("c"), 0);
}
