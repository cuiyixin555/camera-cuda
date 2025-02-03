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
 * Description: defines a bounding box
 ****************************************/
#ifndef CLIM_BOUNDING_BOX_H_
#define CLIM_BOUNDING_BOX_H_
#include <algorithm>
#include <type_traits>

#include "clim/rect.h"

/** @brief defines a number of box representation. */
enum class BoxType {
  /// B-box of [left, top, right, bottom] representation
  xyxy,
  /// B-box of [left, top, width, height] representation
  xywh,
  /// B-box of [x, y, width, height] representation
  /// where x, y is the center point of the box
  cxywh
};

template <BoxType type_, typename T = double>
class BoundingBox : public Rect<T> {
 public:
  /**
   * @brief Construct an empty Bounding Box.
   */
  BoundingBox() = default;

  template <typename U, std::enable_if_t<std::is_arithmetic<U>::value, int> = 0>
  BoundingBox(U x0, U y0, U x1, U y1) : Rect<T>((T)x0, (T)y0, (T)x1, (T)y1) {}

  template <typename U, std::enable_if_t<std::is_arithmetic<U>::value, int> = 0>
  explicit BoundingBox(U (&xyxy)[4]) : Rect<T>(xyxy) {}

  template <typename U>
  explicit BoundingBox(const U& array, size_t off = 0) {
    (*this)[0] = static_cast<T>(array[0 + off]);
    (*this)[1] = static_cast<T>(array[1 + off]);
    (*this)[2] = static_cast<T>(array[2 + off]);
    (*this)[3] = static_cast<T>(array[3 + off]);
  }

  template <typename U>
  BoundingBox<type_, T>& operator=(const Rect<U>& rect) {
    (*this)[0] = static_cast<T>(rect[0]);
    (*this)[1] = static_cast<T>(rect[1]);
    (*this)[2] = static_cast<T>(rect[2]);
    (*this)[3] = static_cast<T>(rect[3]);
    return *this;
  }

  template <typename U>
  BoundingBox<type_, T>& operator=(const std::array<U, 4>& array) {
    (*this)[0] = static_cast<T>(array[0]);
    (*this)[1] = static_cast<T>(array[1]);
    (*this)[2] = static_cast<T>(array[2]);
    (*this)[3] = static_cast<T>(array[3]);
    return *this;
  }

  /**
   * @brief Transform the box representation to BoxType::xywh.
   */
  template <class U = T>
  BoundingBox<BoxType::xywh, U> ToXYWH() const {
    T x, y, w, h;
    switch (type_) {
      case BoxType::xywh:
        return BoundingBox<BoxType::xywh, U>(*this);
      case BoxType::xyxy:
        x = (*this)[0];
        y = (*this)[1];
        w = (*this)[2] - (*this)[0];
        h = (*this)[3] - (*this)[1];
        return BoundingBox<BoxType::xywh, U>(x, y, w, h);
      case BoxType::cxywh:
        x = (*this)[0] - (*this)[2] / 2.0;
        y = (*this)[1] - (*this)[3] / 2.0;
        w = (*this)[2];
        h = (*this)[3];
        return BoundingBox<BoxType::xywh, U>(x, y, w, h);
    }
    return {};
  }

  /**
   * @brief Transform the box representation to BoxType::xyxy.
   */
  template <class U = T>
  BoundingBox<BoxType::xyxy, U> ToXYXY() const {
    T x0, y0, x1, y1;
    switch (type_) {
      case BoxType::xyxy:
        return BoundingBox<BoxType::xyxy, U>(*this);
      case BoxType::xywh:
        x0 = (*this)[0];
        y0 = (*this)[1];
        x1 = (*this)[0] + (*this)[2];
        y1 = (*this)[1] + (*this)[3];
        return BoundingBox<BoxType::xyxy, U>(x0, y0, x1, y1);
      case BoxType::cxywh:
        x0 = (*this)[0] - (*this)[2] / static_cast<T>(2.0);
        y0 = (*this)[1] - (*this)[3] / static_cast<T>(2.0);
        x1 = (*this)[0] + (*this)[2] / static_cast<T>(2.0);
        y1 = (*this)[1] + (*this)[3] / static_cast<T>(2.0);
        return BoundingBox<BoxType::xyxy, U>(x0, y0, x1, y1);
    }
    return {};
  }

  /**
   * @brief Transform the box representation to BoxType::cxywh.
   */
  template <class U = T>
  BoundingBox<BoxType::cxywh, U> ToCXYWH() const {
    T x, y, w, h;
    switch (type_) {
      case BoxType::xywh:
        x = (*this)[0] + (*this)[2] / static_cast<T>(2.0);
        y = (*this)[1] + (*this)[3] / static_cast<T>(2.0);
        w = (*this)[2];
        h = (*this)[3];
        return BoundingBox<BoxType::cxywh, U>(x, y, w, h);
      case BoxType::xyxy:
        x = ((*this)[0] + (*this)[2]) / static_cast<T>(2.0);
        y = ((*this)[1] + (*this)[3]) / static_cast<T>(2.0);
        w = (*this)[2] - (*this)[0];
        h = (*this)[3] - (*this)[1];
        return BoundingBox<BoxType::cxywh, U>(x, y, w, h);
      case BoxType::cxywh:
        return BoundingBox<BoxType::cxywh, U>(*this);
    }
    return {};
  }

  /** @brief get the area of the box. */
  T Area() const {
    switch (type_) {
      case BoxType::xyxy:
        return ((*this)[2] - (*this)[0]) * ((*this)[3] - (*this)[1]);
      case BoxType::xywh:
      case BoxType::cxywh:
        return (*this)[2] * (*this)[3];
    }
    return 0;
  }

  /**
   * @brief Get the intersection area of two boxes.
   *
   * @param others: the other box, must be xyxy type.
   * @return Intersection area.
   */
  template <class U = T>
  T Intersect(const BoundingBox<BoxType::xyxy, U>& others) const {
    BoundingBox<BoxType::xyxy, T> me = this->ToXYXY();
    T max_left = std::max(me[0], others[0]);
    T max_top = std::max(me[1], others[1]);
    T min_right = std::min(me[2], others[2]);
    T min_bottom = std::min(me[3], others[3]);
    if (max_left >= min_right || max_top >= min_bottom) {
      return T(0);
    } else {
      return (min_right - max_left) * (min_bottom - max_top);
    }
  }

  /**
   * @brief Get the union area of two boxes
   *
   * @param others: the other box, must be xyxy type.
   * @return The union area.
   */
  template <class U = T>
  T Union(const BoundingBox<BoxType::xyxy, U>& others) const {
    return Area() + others.Area() - Intersect(others);
  }

  /**
   * @brief Get the jaccard union of two boxes
   *
   * @param others: the other box, must be xyxy type.
   * @return The union area.
   * @note this JACCARD definition is from google mediapipe, which
   * is different from wiki.
   */
  template <class U = T>
  T GoogleJaccard(const BoundingBox<BoxType::xyxy, U>& others) const {
    BoundingBox<BoxType::xyxy, T> me = this->ToXYXY();
    T min_left = std::min(me[0], others[0]);
    T min_top = std::min(me[1], others[1]);
    T max_right = std::max(me[2], others[2]);
    T max_bottom = std::max(me[3], others[3]);
    return (max_right - min_left) * (max_bottom - min_top);
  }

  /** @brief get the width of the box. */
  T Width() const {
    switch (type_) {
      case BoxType::cxywh:
      case BoxType::xywh:
        return (*this)[2];
      case BoxType::xyxy:
      default:
        return (*this)[2] - (*this)[0];
    }
  }

  /** @brief get the height of the box. */
  T Height() const {
    switch (type_) {
      case BoxType::cxywh:
      case BoxType::xywh:
        return (*this)[3];
      case BoxType::xyxy:
      default:
        return (*this)[3] - (*this)[1];
    }
  }

  /** @brief convert to the relative coordinates. */
  BoundingBox<type_, T> ToRelative(T width, T height) const {
    BoundingBox<type_, T> box((*this)[0] / width, (*this)[1] / height,
                              (*this)[2] / width, (*this)[3] / height);
    return box;
  }

  /** @brief convert to the absolute coordinates. */
  BoundingBox<type_, T> ToAbsolute(T width, T height) const {
    BoundingBox<type_, T> box((*this)[0] * width, (*this)[1] * height,
                              (*this)[2] * width, (*this)[3] * height);
    return box;
  }
};
#endif  // CLIM_BOUNDING_BOX_H_
