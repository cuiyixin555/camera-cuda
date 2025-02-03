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
 * Description: bounding box type trait
 ****************************************/
#ifndef CLIM_BBOX_TRAIT_H_
#define CLIM_BBOX_TRAIT_H_
#include <algorithm>
#include <array>
#include <stdexcept>
#include <string>
#include <string_view>
#include <utility>
#include <vector>

#include "clim/bounding_box.h"

inline BoxType BoxTypeFromString(std::string_view str) {
  if (str == "xyxy") return BoxType::xyxy;
  if (str == "xywh") return BoxType::xywh;
  if (str == "cxywh") return BoxType::cxywh;
  throw std::invalid_argument("Unknown box type: " + std::string(str));
}

/**
 * @brief A helper class to iterate over a vector of every fixed values.
 *
 * @tparam T: data type of the container.
 * @tparam V: interval of the iterator.
 */
template <typename T, size_t V>
struct MultiValueIterator {
  using value_type = T;
  using difference_type = std::ptrdiff_t;
  using size_type = typename std::vector<T>::size_type;
  using pointer = typename std::vector<T>::pointer;
  using const_pointer = typename std::vector<T>::const_pointer;

  explicit MultiValueIterator(const std::vector<T>& v)
      : raw_data_(v.data()), ptr_(v.data()), size_(v.size()) {}

  MultiValueIterator(const_pointer p, size_type size)
      : raw_data_(p), ptr_(p), size_(size) {}

  MultiValueIterator& operator++() {
    if (std::distance(raw_data_, ptr_) >= static_cast<difference_type>(size_))
      throw std::out_of_range("Iterator out of range");
    ptr_ += V;
    return *this;
  }

  MultiValueIterator operator++(int) {
    MultiValueIterator tmp(*this);
    ++(*this);
    return tmp;
  }

  MultiValueIterator& operator--() {
    if (ptr_ == raw_data_) throw std::out_of_range("Iterator out of range");
    ptr_ -= V;
    return *this;
  }

  MultiValueIterator operator--(int) {
    MultiValueIterator tmp(*this);
    --(*this);
    return tmp;
  }

  MultiValueIterator& operator+=(size_t n) {
    if (std::distance(raw_data_, ptr_) + static_cast<difference_type>(V * n) >=
        static_cast<difference_type>(size_))
      throw std::out_of_range("Iterator out of range");
    ptr_ += n * V;
    return *this;
  }

  MultiValueIterator& operator-=(size_t n) {
    if (std::distance(raw_data_, ptr_) - static_cast<difference_type>(V * n) <
        0)
      throw std::out_of_range("Iterator out of range");
    ptr_ -= n * V;
    return *this;
  }

  std::array<T, V> operator*() const {
    std::array<T, V> ret;
    std::copy_n(ptr_, V, ret.begin());
    return ret;
  }

  bool operator!() const noexcept {
    return std::distance(raw_data_, ptr_) < static_cast<difference_type>(size_);
  }

  size_type Limits() const noexcept { return size_ / V; }

  const_pointer raw_data_;
  const_pointer ptr_;
  size_type size_;
};

template <typename T>
class BBoxIterator : public MultiValueIterator<T, 4> {
  using BaseClass = MultiValueIterator<T, 4>;
  using const_pointer = typename BaseClass::const_pointer;
  using size_type = typename BaseClass::size_type;

 public:
  explicit BBoxIterator(const std::vector<T>& raw_data,
                        BoxType type = BoxType::xyxy)
      : BaseClass(raw_data), btype_(type) {}

  BBoxIterator(const_pointer p, size_type size, BoxType type = BoxType::xyxy)
      : BaseClass(p, size), btype_(type) {}

  BoundingBox<BoxType::xyxy, T> AtXYXY(size_t pos) const {
    switch (btype_) {
      case BoxType::xywh:
        return AtXYWH(pos).ToXYXY();
      case BoxType::cxywh:
        return AtCXYWH(pos).ToXYXY();
      case BoxType::xyxy:
      default:
        return BoundingBox<BoxType::xyxy, T>(get(pos));
    }
  }

  BoundingBox<BoxType::xywh, T> AtXYWH(size_t pos) const {
    switch (btype_) {
      case BoxType::xyxy:
        return AtXYXY(pos).ToXYWH();
      case BoxType::cxywh:
        return AtCXYWH(pos).ToXYWH();
      case BoxType::xywh:
      default:
        return BoundingBox<BoxType::xywh, T>(get(pos));
    }
  }

  BoundingBox<BoxType::cxywh, T> AtCXYWH(size_t pos) const {
    switch (btype_) {
      case BoxType::xyxy:
        return AtXYXY(pos).ToCXYWH();
      case BoxType::xywh:
        return AtXYWH(pos).ToCXYWH();
      case BoxType::cxywh:
      default:
        return BoundingBox<BoxType::cxywh, T>(get(pos));
    }
  }

 private:
  std::array<T, 4> get(size_t pos) const {
    if (std::distance(BaseClass::raw_data_, BaseClass::ptr_) + pos * 4 >=
        BaseClass::size_) {
      throw std::out_of_range("BBoxIterator: out of bound");
    }
    std::array<T, 4> box;
    std::copy_n(BaseClass::ptr_ + pos * 4, 4, box.begin());
    return box;
  }

  const BoxType btype_;
};
#endif  // CLIM_BBOX_TRAIT_H_
