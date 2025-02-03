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
 * Description: base64 decoding test
 ****************************************/
#include "clim/base64.h"
#include "clim/vt/vt.h"

#include <gtest/gtest.h>

TEST(Base64, Decode) {
  auto binary0 = base64::decode("AA==\n", 5);
  EXPECT_EQ(binary0.size(), 1);
  EXPECT_EQ(std::vector<uint8_t>(binary0.begin(), binary0.begin() + 1),
            std::vector<uint8_t>({0}));
  auto binary1 = base64::decode("AB0sPw==\n", 9);
  EXPECT_EQ(binary1.size(), 4);
  EXPECT_EQ(std::vector<uint8_t>(binary1.begin(), binary1.begin() + 4),
            std::vector<uint8_t>({0x00, 0x1d, 0x2c, 0x3f}));
}

TEST(Base64, IllegalInput) {
  std::string long_text;
  long_text.resize(100000);
  for (int16_t ch = std::numeric_limits<char>::lowest();
       ch <= std::numeric_limits<char>::max(); ch++) {
    std::fill(long_text.begin(), long_text.end(), static_cast<char>(ch));
    base64::decode(long_text.c_str(), long_text.size());
  }
}

TEST(Base64, Encode) {
  auto test = vt::RandomN<std::vector<uint8_t>>({1024});
  auto enc = base64::encode(test.data(), test.size());
  auto dec = base64::decode(enc.data(), enc.size());
  EXPECT_EQ(test, dec);
}
