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
 * Description: defines a rect
 ****************************************/
#ifndef CLIM_RECT_H_
#define CLIM_RECT_H_
#include <array>
#include <cstdlib>

#include "clim/array_arithmetic.h"

template <typename T>
struct Rect : std::array<T, 4> {
  Rect() { this->fill((T)0); }

  Rect(T a, T b, T c, T d) {
    (*this)[0] = a;
    (*this)[1] = b;
    (*this)[2] = c;
    (*this)[3] = d;
  }

  explicit Rect(T (&x)[4]) {
    (*this)[0] = x[0];
    (*this)[1] = x[1];
    (*this)[2] = x[2];
    (*this)[3] = x[3];
  }

  template <typename U>
  Rect<T>& operator=(const std::array<U, 4>& array) {
    (*this)[0] = static_cast<T>(array[0]);
    (*this)[1] = static_cast<T>(array[1]);
    (*this)[2] = static_cast<T>(array[2]);
    (*this)[3] = static_cast<T>(array[3]);
    return *this;
  }
};

template <typename T>
struct Point : std::array<T, 2> {
  Point() { this->fill((T)0); }

  Point(T a, T b) {
    (*this)[0] = a;
    (*this)[1] = b;
  }

  explicit Point(T (&x)[2]) {
    (*this)[0] = x[0];
    (*this)[1] = x[1];
  }

  template <typename U>
  Point<T>& operator=(const std::array<U, 2>& array) {
    (*this)[0] = static_cast<T>(array[0]);
    (*this)[1] = static_cast<T>(array[1]);
    return *this;
  }

  T X() const { return (*this)[0]; }
  T Y() const { return (*this)[1]; }
  T& X() { return (*this)[0]; }
  T& Y() { return (*this)[1]; }
};

using Rectd = Rect<double>;
using Rectf = Rect<float>;
using Recti = Rect<int32_t>;

using Pointd = Point<double>;
using Pointf = Point<float>;
using Pointi = Point<int32_t>;
#endif  // CLIM_RECT_H_
