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
 * Description: Math functions for simple algebra
 ****************************************/
#ifndef CLIM_NUMERICAL_H_
#define CLIM_NUMERICAL_H_
#include <algorithm>
#include <cmath>
#include <limits>
#include <type_traits>

/// NOLINTBEGIN(readability-identifier-naming)

/**
 * @brief rounded up integer division
 *
 * @tparam T: data type, must be an integer type.
 * @param a: dividend.
 * @param b: divisor.
 * @return y = round_up(a / b)
 */
template <class T, std::enable_if_t<std::is_integral_v<T>, int> = 0>
inline constexpr T round_div(T a, T b) {
  return (a + b - 1) / b;
}

/**
 * @brief numeric division for any input type
 *
 * @tparam T: output data type, must be a floating point.
 * @tparam U1: dividend type.
 * @tparam U2: divisor type.
 * @param a: dividend.
 * @param b: divisor.
 * @return y = a / b
 */
template <class T, class U1, class U2,
          std::enable_if_t<std::is_floating_point_v<T>, int> = 0>
inline constexpr T numeric_div(U1 a, U2 b) {
  T tmp = static_cast<T>(b);
  if (fabs(tmp) <= std::numeric_limits<T>::epsilon()) {
    tmp = std::numeric_limits<T>::epsilon() * (tmp < 0 ? -1.0f : 1.0f);
  }
  return static_cast<T>(a) / tmp;
}

/**
 * @brief rounded down an integer to be divisible by the aligned integer.
 *
 * @tparam T: data type, must be an integer.
 * @param value: input integer.
 * @param align_value: aligned integer.
 * @return y that y mod `align_value` === 0.
 */
template <class T, std::enable_if_t<std::is_integral_v<T>, int> = 0>
inline constexpr T align_floor(T value, T align_value) {
  return value / align_value * align_value;
}

/**
 * @brief rounded up an integer to be divisible by the aligned integer.
 *
 * @tparam T: data type, must be an integer.
 * @param value: input integer.
 * @param align_value: aligned integer.
 * @return y that y mod `align_value` === 0.
 * @note this will align an divisible integer to a larger one.
 * i.e. align_ceil(0, 4)=4, align_ceil(4, 4)=8.
 */
template <class T, std::enable_if_t<std::is_integral_v<T>, int> = 0>
inline constexpr T align_ceil(T value, T align_value) {
  return align_floor(value + align_value, align_value);
}

/**
 * @brief rounded up an integer to be divisible by the aligned integer.
 *
 * @tparam T: data type, must be an integer.
 * @param value: input integer.
 * @param align_value: aligned integer.
 * @return y that y mod `align_value` === 0.
 */
template <class T, std::enable_if_t<std::is_integral_v<T>, int> = 0>
inline constexpr T align(T value, T align_value) {
  return round_div(value, align_value) * align_value;
}

/**
 * @brief clip the input value between the boundary.
 *
 * @tparam T: data type.
 * @param x: input value.
 * @param low: lower boundary.
 * @param high: higher boundary.
 * @return a value belongs [low, high].
 */
template <typename T>
inline constexpr T clip(const T x,
                        const T low = (std::numeric_limits<T>::min)(),
                        const T high = (std::numeric_limits<T>::max)()) {
  return std::max(std::min(x, high), low);
}

/// NOLINTEND(readability-identifier-naming)
#endif  // CLIM_NUMERICAL_H_
