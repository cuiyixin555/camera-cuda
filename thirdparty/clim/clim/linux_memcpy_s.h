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
 * Description: Linux's memcpy_s
 ****************************************/
#ifndef CLIM_LINUX_MEMCPY_S_H_
#define CLIM_LINUX_MEMCPY_S_H_
#include <cstring>

#ifndef _WIN32
/// NOLINTNEXTLINE(readability-identifier-naming)
inline void memcpy_s(void* dst, size_t dst_size, const void* src,
                     size_t src_size) {
  if (src_size < dst_size) {
    std::memcpy(dst, src, src_size);
  } else {
    std::memcpy(dst, src, dst_size);
  }
}
#endif

/// NOLINTNEXTLINE(readability-identifier-naming)
inline void truncated_memcpy(void* dst, size_t dst_size, const void* src,
                             size_t src_size) {
  if (src_size < dst_size) {
    memcpy_s(dst, dst_size, src, src_size);
  } else {
    memcpy_s(dst, dst_size, src, dst_size);
  }
}
#endif  // CLIM_LINUX_MEMCPY_S_H_
