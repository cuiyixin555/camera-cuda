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
 * Description: split string widh sep char
 ****************************************/
#ifndef CLIM_STR_SPLIT_H_
#define CLIM_STR_SPLIT_H_
#include <string>
#include <string_view>
#include <vector>
#include <immintrin.h>


/**
 * @brief splitting string with a separate character to at most two parts
 *
 * @param s: input string
 * @param sep: a separate character
 * @return a list of separated string
 */
inline std::vector<std::string_view> StrSplit2StringViewAVX(std::string_view s,
                                                            char sep) {
  // output has to allocate real buffer because input string view
  // may be an rvalue.
  std::vector<std::string_view> ret;
  if (s.empty()) return ret;
  if (sep=='\0') return {s};
  ret.reserve(2);

  __m256i sep_vector = _mm256_set1_epi8(sep);
  size_t index = 0;
  size_t length = s.size();

  while (index < length){
    __m256i chunk = _mm256_load_si256(reinterpret_cast<const __m256i*>(s.data()+index));
    __m256i result = _mm256_cmpeq_epi8(chunk, sep_vector);
    int mask = _mm256_movemask_epi8(result);

    if (mask != 0) {
      int pos = _tzcnt_u32((unsigned int)mask);
      if (index + pos > length){
        break;
      }

      ret.emplace_back(s.data(), pos + index);
      ret.emplace_back(s.data() + pos + index + 1);
      return ret;
    }
    index += 32;
  }

  ret.emplace_back(s.data(), length);
  return ret;
}

/**
 * @brief splitting string with a separate string
 *
 * @param s: input string
 * @param sep: a separate string
 * @return a list of separated string
 */
inline std::vector<std::string_view> StrSplitStringView(std::string_view s,
                                                        std::string_view sep) {
  // output has to allocate real buffer because input string view
  // may be an rvalue.
  std::vector<std::string_view> ret;
  std::string_view substr = s;
  if (s.empty()) return ret;
  if (sep.empty()) return {s};
  ret.reserve(2);

  while (!substr.empty()) {
    auto pos = substr.find(sep, 0);
    if (pos > substr.size()) {
      ret.push_back(substr);
      break;
    }
    ret.emplace_back(substr.data(), pos);
    substr = substr.substr(pos + sep.size());
  }
  return ret;
}

/**
 * @brief splitting string with a separate character
 *
 * @param s: input string
 * @param sep: a separate character
 * @return a list of separated string
 */
inline std::vector<std::string_view> StrSplitStringView(std::string_view s,
                                                        char sep) {
  // output has to allocate real buffer because input string view
  // may be an rvalue.
  std::vector<std::string_view> ret;
  std::string_view substr = s;
  if (s.empty()) return ret;
  if (sep=='\0') return {s};
  ret.reserve(2);

  while (!substr.empty()) {
    auto pos = substr.find(sep, 0);
    if (pos > substr.size()) {
      ret.push_back(substr);
      break;
    }
    ret.emplace_back(substr.data(), pos);
    substr = substr.substr(pos + 1);
  }
  return ret;
}

/**
 * @brief splitting string with a separate string or a separate char
 *
 * Same as StrSplit but returns a vector of std::string.
 * @param s: input string
 * @param sep: a separate string or a separate char
 * @return a list of separated string
 */
template <typename T>

std::vector<std::string> StrSplit(std::string_view s, T sep) {
  std::vector<std::string_view> v = StrSplitStringView(s, sep);
  return std::vector<std::string>(v.begin(), v.end());
}
#endif  // CLIM_STR_SPLIT_H_
