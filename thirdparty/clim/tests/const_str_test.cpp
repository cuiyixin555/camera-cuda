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
#include <gtest/gtest.h>

#include <string_view>

#include "clim/reflect.h"

TEST(SV, Search) {
  constexpr std::string_view a("a/b/c/d/e/const_str_test.cpp");
  constexpr size_t p = a.rfind('/');
  constexpr size_t q = a.find('.');
  constexpr auto b = a.substr(p + 1);

  static_assert(b == "const_str_test.cpp");
  constexpr auto c = a.substr(q + 1);
  static_assert(c == "cpp");
  constexpr auto d = a.substr(p + 1, q - p - 1);
  static_assert(d == "const_str_test");
}

TEST(SV, SearchFail) {
  constexpr std::string_view a("a/b/c/d/e/const_str_test.cpp");
  constexpr size_t reverse_not_found = a.rfind('\\');
  EXPECT_EQ(reverse_not_found, a.npos);
  constexpr size_t not_found = a.find('\\');
  EXPECT_EQ(not_found, a.npos);
}

TEST(Reflect, Filename) {
  auto test_name1 = FILESTEM(__FILE__);
  EXPECT_EQ(std::string(test_name1), "const_str_test");
  auto test_name2 = BASENAME(__FILE__);
  EXPECT_EQ(std::string(test_name2), "const_str_test.cpp");
#ifdef _WIN32
  auto test_name3 = FILESTEM("C:\\D\\E\\F");
  auto test_name4 = FILESTEM("C:\\D\\E\\F.cpp");
  auto test_name5 = FILESTEM("C:\\user.CCR\\D\\E\\F.cpp");
  auto name_cant_parsed = FILESTEM("C/D/E/F");
  auto name_cant_parsed_1 = FILESTEM("C");
  EXPECT_EQ(std::string(name_cant_parsed), "C/D/E/F");
#else
  auto test_name3 = FILESTEM("C/D/E/F");
  auto test_name4 = FILESTEM("/C/D/E/F.cpp");
  auto test_name5 = FILESTEM("/C/user.CCR/E/F.cpp");
  auto name_cant_parsed = FILESTEM("C:\\D\\E\\F");
  auto name_cant_parsed_1 = FILESTEM("C");
  EXPECT_EQ(std::string(name_cant_parsed), "C:\\D\\E\\F");
#endif
  EXPECT_EQ(std::string(test_name3), "F");
  EXPECT_EQ(std::string(test_name4), "F");
  EXPECT_EQ(std::string(test_name5), "F");
  EXPECT_EQ(std::string(name_cant_parsed_1), "C");
}

#define THROUGH_MACRO(...) FILESTEM(__FILE__)

TEST(Reflect, ThroughMacro) {
  auto test_name = THROUGH_MACRO(__FILE__);
  EXPECT_EQ(std::string(test_name), "const_str_test");
}
