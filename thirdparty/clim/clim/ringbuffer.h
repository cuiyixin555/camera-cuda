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
 * Description: a ring buffer fifo template
 ****************************************/
#include <memory>
#include <type_traits>

/**
 * @brief A simple ring buffer object, acts like a FIFO.
 *
 * This ring buffer is not thread-safe.
 * @tparam T: the object type contains in the ring buffer.
 */
template <class T>
class RingBuffer {
  static_assert(std::is_nothrow_default_constructible<T>::value,
                "Element type must be nothrow default constructible");

 public:
  using value_type = T;

  /**
   * @brief Construct a new Ring Buffer object with a fixed size.
   *
   * @param size: capacity of the ring buffer, default to 2.
   */
  explicit RingBuffer(size_t size = 2) : capacity_(size) {
    int length = 1;
    while (length < size) length <<= 1;
    mask = length - 1; // should be like 0b0011
    buffer_.reset(new T[length]);
  }

  ~RingBuffer() = default;

  /// Get the ringbuffer capacity.
  size_t Capacity() const { return capacity_; }

  /**
   * @brief Add an element to the ring buffer.
   *
   * If the ring buffer is full, the oldest element will be removed (roll-over).
   * @param data: the element to be added.
   */
  void Push(const T& data) noexcept {
    buffer_[head_ & mask] = data;
    head_++;
    if (head_ == tail_ + capacity_ + 1) {
      // new data overflow, override the oldest data.
      tail_++;
    }
  }

  /**
   * @brief Read the oldest element from the ring buffer.
   *
   * The method is an UB if the ring buffer is empty.
   * @return T: the oldest element.
   */
  T& Peak() noexcept {
    T& data = buffer_[tail_ & mask];
    return data;
  }

  /**
   * @brief Read the oldest element from the ring buffer and remove it.
   *
   * @return T: the element read from.
   */
  T Pop() noexcept {
    T& data = Peak();
    if (!Empty()) {
      tail_++;
    }
    return data;
  }

  void PopAndDel() noexcept {
    if (!Empty()) {
      tail_++;
    }
  }

  /// Check if the ring buffer is empty.
  inline bool Empty() const { return head_ == tail_; }

 private:
  std::unique_ptr<T[]> buffer_;
  size_t capacity_;
  size_t mask;
  size_t head_ = 0;
  size_t tail_ = 0;
};
