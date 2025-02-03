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
 * Description: strip strings
 ****************************************/
#ifndef CLIM_STRIP_H_
#define CLIM_STRIP_H_
#include <string>
#include <string_view>
#include <utility>

inline std::string Strip(std::string_view str, char t = ' ') {
  size_t beg = str.find_first_not_of(t, 0);
  if (beg == std::string_view::npos) beg = 0;
  size_t end = str.find_last_not_of(t);
  if (end == std::string_view::npos) end = str.size() - 1;
  return std::string(str.substr(beg, end - beg + 1));
}
#endif  // CLIM_STRIP_H_
