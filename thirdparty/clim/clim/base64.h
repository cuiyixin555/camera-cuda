//
// Copyright (c) 2016-2019 Vinnie Falco (vinnie dot falco at gmail dot com)
//
// Distributed under the Boost Software License, Version 1.0. (See accompanying
// file LICENSE_1_0.txt or copy at http://www.boost.org/LICENSE_1_0.txt)
//
// Official repository: https://github.com/boostorg/beast
//
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
 * Description: Modified boost base64.hpp
 ****************************************/
#ifndef CLIM_BASE64_H_
#define CLIM_BASE64_H_

#include <cctype>
#include <cstdint>
#include <string>
#include <vector>

namespace base64 {
/// NOLINTBEGIN(*-magic-numbers)

/**
 * @brief Decode a padded base64 string into binary
 *
 * @param src: encoded string
 * @param size: encoded string length
 * @return std::vector<uint8_t>
 * NOLINTNEXTLINE(readability-*-naming) */
inline std::vector<uint8_t> decode(char const* src, std::size_t size) {
  std::vector<unsigned char> out;
  const auto* in = reinterpret_cast<unsigned char const*>(src);
  unsigned char c3[3], c4[4];  // NOLINT(*-avoid-c-arrays)
  int i = 0;
  int j = 0;
  int64_t len = static_cast<int64_t>(size & 0x7FFFFFFFFFFFFFFFULL);

  // NOLINTNEXTLINE(*-avoid-c-arrays)
  static signed char constexpr kTab[] = {
      -1, -1, -1, -1, -1, -1, -1, -1,
      -1, -1, -1, -1, -1, -1, -1, -1,  //   0-15
      -1, -1, -1, -1, -1, -1, -1, -1,
      -1, -1, -1, -1, -1, -1, -1, -1,  //  16-31
      -1, -1, -1, -1, -1, -1, -1, -1,
      -1, -1, -1, 62, -1, -1, -1, 63,  //  32-47
      52, 53, 54, 55, 56, 57, 58, 59,
      60, 61, -1, -1, -1, -1, -1, -1,  //  48-63
      -1, 0,  1,  2,  3,  4,  5,  6,
      7,  8,  9,  10, 11, 12, 13, 14,  //  64-79
      15, 16, 17, 18, 19, 20, 21, 22,
      23, 24, 25, -1, -1, -1, -1, -1,  //  80-95
      -1, 26, 27, 28, 29, 30, 31, 32,
      33, 34, 35, 36, 37, 38, 39, 40,  //  96-111
      41, 42, 43, 44, 45, 46, 47, 48,
      49, 50, 51, -1, -1, -1, -1, -1,  // 112-127
      -1, -1, -1, -1, -1, -1, -1, -1,
      -1, -1, -1, -1, -1, -1, -1, -1,  // 128-143
      -1, -1, -1, -1, -1, -1, -1, -1,
      -1, -1, -1, -1, -1, -1, -1, -1,  // 144-159
      -1, -1, -1, -1, -1, -1, -1, -1,
      -1, -1, -1, -1, -1, -1, -1, -1,  // 160-175
      -1, -1, -1, -1, -1, -1, -1, -1,
      -1, -1, -1, -1, -1, -1, -1, -1,  // 176-191
      -1, -1, -1, -1, -1, -1, -1, -1,
      -1, -1, -1, -1, -1, -1, -1, -1,  // 192-207
      -1, -1, -1, -1, -1, -1, -1, -1,
      -1, -1, -1, -1, -1, -1, -1, -1,  // 208-223
      -1, -1, -1, -1, -1, -1, -1, -1,
      -1, -1, -1, -1, -1, -1, -1, -1,  // 224-239
      -1, -1, -1, -1, -1, -1, -1, -1,
      -1, -1, -1, -1, -1, -1, -1, -1  // 240-255
  };
  const auto* const kInverse = &kTab[0];

  do {
    while (len-- && *in != '=') {
      auto const kV = kInverse[*in];
      if (kV == -1) break;
      ++in;
      c4[i] = kV;
      if (++i == 4) {
        c3[0] = (c4[0] << 2) + ((c4[1] & 0x30) >> 4);
        c3[1] = ((c4[1] & 0xf) << 4) + ((c4[2] & 0x3c) >> 2);
        c3[2] = ((c4[2] & 0x3) << 6) + c4[3];

        for (i = 0; i < 3; i++) out.push_back(c3[i]);
        i = 0;
      }
    }

    if (i) {
      c3[0] = (c4[0] << 2) + ((c4[1] & 0x30) >> 4);
      c3[1] = ((c4[1] & 0xf) << 4) + ((c4[2] & 0x3c) >> 2);
      c3[2] = ((c4[2] & 0x3) << 6) + c4[3];

      for (j = 0; j < i - 1; j++) out.push_back(c3[j]);
      i = 0;
    }
    if (*in != '=') {
      ++in;
    }
  } while (len > 0);

  return out;
}

/**
 * @brief Encode a series of octets as a padded, base64 string.
 * The resulting string will not be null terminated.
 *
 * @param src: byte start pointer.
 * @param len: byte length.
 * @return The encoded string.
 * NOLINTNEXTLINE(readability-*-naming) */
inline std::string encode(void const* src, size_t len) {
  // NOLINTNEXTLINE(*-avoid-c-arrays)
  static char constexpr kTab[] = {
      "ABCDEFGHIJKLMNOP"
      "QRSTUVWXYZabcdef"
      "ghijklmnopqrstuv"
      "wxyz0123456789+/"};

  std::string out;
  out.reserve(4 * ((len + 2) / 3));
  char const* in = static_cast<char const*>(src);

  for (auto n = len / 3; n--;) {
    out.push_back(kTab[(in[0] & 0xfc) >> 2]);
    out.push_back(kTab[((in[0] & 0x03) << 4) + ((in[1] & 0xf0) >> 4)]);
    out.push_back(kTab[((in[2] & 0xc0) >> 6) + ((in[1] & 0x0f) << 2)]);
    out.push_back(kTab[in[2] & 0x3f]);
    in += 3;
  }

  switch (len % 3) {
    case 2:
      out.push_back(kTab[(in[0] & 0xfc) >> 2]);
      out.push_back(kTab[((in[0] & 0x03) << 4) + ((in[1] & 0xf0) >> 4)]);
      out.push_back(kTab[(in[1] & 0x0f) << 2]);
      out.push_back('=');
      break;

    case 1:
      out.push_back(kTab[(in[0] & 0xfc) >> 2]);
      out.push_back(kTab[((in[0] & 0x03) << 4)]);
      out.push_back('=');
      out.push_back('=');
      break;

    case 0:
      break;
  }

  return out;
}
/// NOLINTEND(*-magic-numbers)
}  // namespace base64
#endif  // CLIM_BASE64_H_
