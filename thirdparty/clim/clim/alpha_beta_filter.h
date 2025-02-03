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
 * Description: Alpha-Beta Filter
 ****************************************/
#ifndef CLIM_ALPHA_BETA_FILTER_H_
#define CLIM_ALPHA_BETA_FILTER_H_
#include <array>

#include "clim/array_arithmetic.h"

/**
 * @brief Alpha beta filter for an array.
 *
 * Alpha-beta filter is a simplified form of observer for estimation, data
 * smoothing and control applications. It is closely related to Kalman filters
 * and to linear state observers used in control theory. Its principal advantage
 * is that it does not require a detailed system model.
 *
 * https://en.wikipedia.org/wiki/Alpha_beta_filter
 * @tparam size: array size.
 * @tparam T: array value type.
 */
template <size_t size, typename T = double,
          std::enable_if_t<std::is_floating_point<T>::value, int> = 0>
class AlphaBetaFilter {
  using value_type = std::array<T, size>;

 public:
  AlphaBetaFilter() : AlphaBetaFilter(0, 0) {}

  AlphaBetaFilter(T s, T v) : startup_(false) {
    state_.fill(s);
    velocity_.fill(v);
  }

  // NOLINTNEXTLINE(*-c-arrays)
  AlphaBetaFilter(T (&s)[size], T (&v)[size]) : startup_(false) {
    for (size_t i = 0; i < size; i++) {
      state_[i] = s[i];
      velocity_[i] = v[i];
    }
  }

  AlphaBetaFilter(value_type s, value_type v)
      : state_(s), velocity_(v), startup_(false) {}

  /**
   * @brief Filtering the value
   * Provide $ val_smoothed = val * \alpha + (1 - \alpha) * estimated $.
   *
   * @param val : value to smooth
   * @param alpha : blending factor between estimated and current value
   * @param beta : acceralation of velocity
   * @param dt : time step
   * @return filtered value
   */
  value_type Filter(value_type val, value_type alpha, value_type beta, T dt) {
    value_type new_state = Predict(dt);
    if (startup_) {
      state_ = alpha * val + (T(1.0) - alpha) * new_state;
      velocity_ += beta * (val - new_state) / dt;
    } else {
      state_ = val;
      startup_ = true;
    }
    return state_;
  }

  /**
   * @brief Predict the next state in `dt` time
   *
   * @param dt : time in advance
   * @return predicted state
   */
  value_type Predict(T dt) const { return state_ + velocity_ * dt; }

  /** @brief get the velocity. */
  value_type Velocity() const { return velocity_; }

  /** Reset the filter */
  void Reset(bool state = true, bool velocity = true) {
    if (state) state_.fill(0);
    if (velocity) velocity_.fill(0);
    if (state && velocity) startup_ = false;
  }

  /**
   * @brief See AlphaBetaFilter::Filter
   */
  value_type operator()(value_type val, T alpha, T beta, T dt) {
    value_type v_alpha, v_beta;
    v_alpha.fill(alpha);
    v_beta.fill(beta);
    return this->Filter(val, v_alpha, v_beta, dt);
  }

  // NOLINTNEXTLINE(*-c-arrays)
  value_type operator()(T (&val)[size], T alpha, T beta, T dt) {
    value_type x;
    size_t pos = 0;
    std::for_each(x.begin(), x.end(), [&](T& v) { v = val[pos++]; });
    return this->Filter(x, alpha, beta, dt);
  }

  value_type operator()(value_type val, value_type alpha, value_type beta,
                        T dt) {
    return this->Filter(val, alpha, beta, dt);
  }

 private:
  value_type velocity_;
  value_type state_;
  bool startup_;
};
#endif  // CLIM_ALPHA_BETA_FILTER_H_
