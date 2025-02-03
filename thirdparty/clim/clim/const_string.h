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
 * Description: a class for constant string
 ****************************************/
#ifndef CLIM_CONST_STRING_H_
#define CLIM_CONST_STRING_H_
#include <algorithm>
#include <cstdlib>

/**
 * @brief A compile-time constant string. All members and methods are const
 * evaluated. Useful for macro string parsing.
 *
 * @tparam Char: string charactor type, usually char or wchar_t
 * @tparam Length: string length, note the empty string length is 1 ("\0")
 *
 * @example
 * ConstString("abc").Size();  // 4
 * ConstString("abc").Data();  // "abc"
 * ConstString("abc")[1];  // 'b'
 * ConstString("abc").Substr(1, 2);  // ConstString("bc")
 * ConstString("cabc").ReverseFind('c');  // 2
 * ConstString("cabc").ReverseFind('d');  // 0
 * ConstString("cabc").Find('c');  // 0
 * ConstString("cabc").Find('d');  // 5
 */
template <typename Char, size_t Length>
class ConstString {
  static_assert(Length != 0, "zero length buffer is not allowed");

 public:
  constexpr ConstString() = default;

  /**
   * @brief Make a ConstString from a const literal.
   *
   * E.g ConstString(__FUNCTION__);
   * NOLINTNEXTLINE(*-avoid-c-arrays) */
  explicit constexpr ConstString(const Char (&ch)[Length]) {
    for (size_t i = 0; i < Length; i++) {
      data_[i] = ch[i];
    }
  }

  /**
   * @brief Make a ConstString from a pair of begin and end pointer.
   */
  constexpr ConstString(const Char *beg, const Char *end) {
    size_t i = 0;
    for (const Char *it = beg; it != end; it++, i++) {
      if (i == Length - 1) break;
      data_[i] = *it;
    }
    data_[Length - 1] = 0;
  }

  constexpr ConstString(const ConstString &rstr) {
    for (size_t i = 0; i < Length; i++) {
      data_[i] = rstr.data_[i];
    }
  }

  /// @brief Access the const string element.
  constexpr auto operator[](size_t pos) const { return data_[pos]; }

  template <size_t N>
  constexpr bool operator==(const ConstString<Char, N> &rstr) const {
    if constexpr (N != Length) return false;
    for (size_t i = 0; i < Length; i++) {
      if (data_[i] != rstr[i]) return false;
    }
    return true;
  }

  /// @return the start pointer of the const string.
  const Char *Data() const { return &data_[0]; }

  /// @return the length of the string. End charactor ('\0') is counted.
  constexpr size_t Size() const { return Length; }

  /**
   * @brief Find the first occurence of a substring.
   *
   * @param substr: the substring to find
   * @return the offset position of the const string, or the string length if
   *   not found.
   */
  template <size_t N>
  constexpr size_t Find(const ConstString<Char, N> &substr) const {
    for (size_t i = 0; i < Length - 1; i++) {
      for (size_t j = 0; j < N - 1; j++) {
        if (data_[i] == substr[j]) {
          if (j == N - 2) return i + 2 - N;
          i++;
        } else {
          break;
        }
      }
    }
    return Length;
  }

  /**
   * @brief Find the last occurence of a substring.
   *
   * @param substr: the substring to find
   * @return the offset position of the const string, or Length if not found.
   */
  template <size_t N>
  constexpr size_t ReverseFind(const ConstString<Char, N> &substr) const {
    if constexpr (N == 1) return Length - 1;
    bool equal = true;
    for (size_t i = Length - 2; i > 0; i--) {
      equal = true;
      for (size_t j = N - 2; j > 0; j--) {
        if (data_[i] == substr[j]) {
          i--;
        } else {
          equal = false;
          break;
        }
      }
      if (equal && data_[i] == substr[0]) return i;
    }
    if (equal && data_[0] == substr[0]) return 0;
    return Length;
  }

  /**
   * @brief Make a substring of the const string.
   *
   * @tparam P: the start position of the substring
   * @tparam N: the length of the substring
   * @return constexpr ConstString<Char, N + 1>
   */
  template <size_t P, size_t N = Length - P - 1>
  constexpr auto Substr() const {
    constexpr size_t kOffset = (P >= 0 && P < Length) ? P : 0;
    constexpr size_t kSize =
        (kOffset + N >= Length) ? (Length - kOffset - 1) : N;
    return ConstString<Char, kSize + 1>(data_ + kOffset,
                                        data_ + kOffset + kSize);
  }

 private:
  Char data_[Length] = {};  // NOLINT(*-avoid-c-arrays)
};
#endif  // CLIM_CONST_STRING_H_
