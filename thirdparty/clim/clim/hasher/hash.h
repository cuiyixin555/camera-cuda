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
 * Description: hash functions
 ****************************************/
#ifndef CLIM_HASHER_HASH_H_
#define CLIM_HASHER_HASH_H_
#include <string_view>
#include <type_traits>
#include <utility>

#include "clim/hasher/city.h"
#include "clim/hasher/murmur3.h"

/**
 * @brief A helper class implement string hash using several hash functions.
 *
 * For string length >= 16, it is city hasher 64bit, for other cases it is
 * original std::hash.
 */
struct MixStringHash {
  uint64_t operator()(std::string_view key) const noexcept {
    if (key.size() >= 16) {
      return CityHash64(key.data(), key.size());
    } else {
      const std::hash<std::string_view> hasher;
      return hasher(key);
    }
  }
};
#endif  // CLIM_HASHER_HASH_H_
