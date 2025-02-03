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
 * Description: replace string
 ****************************************/
#include <string>
#include <string_view>

/**
 * @brief Replace all matched parts of a string with another string.
 *
 * The replacement will change the original string object.
 *
 * @param [inout] str: the string to be replaced.
 * @param [in] what: the part to replace.
 * @param [in] with: the replacement string.
 * @return std::string_view of the replaced string.
 */
inline std::string_view StrReplaceInplace(std::string& str,
                                          std::string_view what,
                                          std::string_view with) {
  for (size_t pos = 0; pos < str.size();) {
    if (str.substr(pos, what.size()) == what) {
      str.replace(pos, what.size(), with);
      pos += with.size();
    } else {
      ++pos;
    }
  }
  return str;
}

/**
 * @brief Replace all matched parts of a string with another string.
 *
 * The replacement won't change the original string object.
 *
 * @param [in] str: the string to be replaced.
 * @param [in] what: the part to replace.
 * @param [in] with: the replacement string.
 * @return The replaced string.
 */
inline std::string StrReplace(std::string_view str, std::string_view what,
                              std::string_view with) {
  std::string ret(str);
  StrReplaceInplace(ret, what, with);
  return ret;
}
