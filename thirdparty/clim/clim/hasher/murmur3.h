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
 * Description: murmur3hasher
 ****************************************/
#ifndef CLIM_HASHER_MURMUR3_H_
#define CLIM_HASHER_MURMUR3_H_
#ifdef _MSC_VER
#pragma warning(push)
#pragma warning(disable : 4505)
#endif
#include <string>

#include "clim/hasher/murmur3_internal.h"

static uint64_t MurmurHash3(const void* key, int len, uint32_t seed) {
  uint64_t t;
  murmur3::internal::MurmurHash3_x86_32(key, len, seed, &t);
  return t;
}
#ifdef _MSC_VER
#pragma warning(pop)
#endif

namespace murmur3 {
template <class T>
struct Hash {
  uint64_t operator()(const T& key) const noexcept {
    static_assert(std::is_trivial_v<T>);
    return MurmurHash3(reinterpret_cast<const char*>(&key), sizeof(T), 0);
  }
};

template <>
struct Hash<std::string> {
  uint64_t operator()(const std::string& key) const noexcept {
    return MurmurHash3(key.c_str(), static_cast<int>(key.size()), 0);
  }
};
}  // namespace murmur3
#endif  // CLIM_HASHER_MURMUR3_H_
