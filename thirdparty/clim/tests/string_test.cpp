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
 * Description: test string functions
 ****************************************/
#include <gtest/gtest.h>

#include "clim/str_replace.h"
#include "clim/str_split.h"
#include "clim/strip.h"

TEST(String, StripTest) {
  EXPECT_EQ(Strip("   abcd   "), "abcd") << "Strip abcd";
  EXPECT_EQ(Strip("hello world"), "hello world") << "Strip hello world";
  EXPECT_EQ(Strip(""), "") << "Strip empty string";
}

TEST(String, SplitTest) {
  EXPECT_EQ(StrSplit("1,2,3,4", ','),
            std::vector<std::string>({"1", "2", "3", "4"}))
      << "single sep";
  EXPECT_EQ(StrSplit("1::2::3::4", "::"),
            std::vector<std::string>({"1", "2", "3", "4"}))
      << "double sep";
  EXPECT_EQ(StrSplit("", '.'), std::vector<std::string>()) << "empty string";
  EXPECT_EQ(StrSplit("", ""), std::vector<std::string>()) << "empty sep";
  EXPECT_EQ(StrSplit("abc", ""), std::vector<std::string>({"abc"}))
      << "empty sep";
}

TEST(String, ReplaceTest) {
  std::string test_str("alice loves bob and bob does not love alice");
  auto ans1 = StrReplace(test_str, "bob", "clark");
  EXPECT_EQ(ans1, "alice loves clark and clark does not love alice");
  EXPECT_NE(ans1, test_str);
  auto ans2 = StrReplaceInplace(test_str, "love", "hate");
  EXPECT_EQ(ans2, test_str);
  EXPECT_EQ(ans2, "alice hates bob and bob does not hate alice");
  EXPECT_EQ(StrReplace("helloworld", "hello", "prependhello"),
            "prependhelloworld");
  EXPECT_EQ(StrReplace("helloworld", "hello", ""), "world");
  EXPECT_EQ(StrReplace("hello world", "o", "lo"), "helllo wlorld");
}
