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
 * Description: Kalman filter
 ****************************************/
#ifndef CLIM_KALMAN_FILTER_H_
#define CLIM_KALMAN_FILTER_H_
#include <array>

#include "clim/array_arithmetic.h"

template <size_t size, typename T = double,
          std::enable_if_t<std::is_floating_point<T>::value, int> = 0>
class KalmanFilter {
  using value_type = std::array<T, size>;

  struct State {
    std::array<T, 2> S = {};
    std::array<T, 4> P = {};
  };

 public:
  KalmanFilter(const value_type& z, const value_type& var) {
    for (size_t i = 0; i < size; i++) {
      states_[i].S[0] = z[i];
      states_[i].P[0] = var[i];
    }
  }

  value_type Priori() {
    value_type ret{};
    for (size_t i = 0; i < size; i++) {
      ret[i] = states_[i].S[0];
    }
    return ret;
  }

  value_type Posteriori(const value_type& measure, T dt, const value_type& R,
                        const std::array<std::array<T, 4>, size>& Q) {
    value_type ans{};
    for (size_t i = 0; i < size; i++) {
      std::array<T, 4> A{1, dt, 0, 1};
      std::array<T, 2> hs;
      std::array<T, 4> AP, APAt;
      std::array<T, 4> hP;
      std::array<T, 1> y;
      std::array<T, 1> S;
      std::array<T, 2> K;
      std::array<T, 4> I_KH;

      // 1. predict state: `s = A*s`
      simpleMatmul<T, 2, 2, 2, 1>(hs.data(), A.data(), states_[i].S.data());

      // 2. predict estimate covariance
      //  - P = A*P*At + Q
      simpleMatmul<T, 2, 2, 2, 2>(AP.data(), A.data(), states_[i].P.data());
      simpleMatmul<T, 2, 2, 2, 2>(APAt.data(), AP.data(), A.data(), true);
      hP = APAt + Q[i];

      // 3. compute measurement residual
      //  - y = z - H*hx
      y[0] = measure[i] - hs[0];

      // 4. compute residual covariance
      //  - S = H*P*Ht + R
      S[0] = hP[0] + R[i];

      // 5. compute optimal kalman gain
      //  - K = P*Ht*inv(S)
      K[0] = hP[0];
      K[1] = hP[2];

      // 6. update state
      //  - x = x + K*y
      states_[i].S[0] = hs[0] + K[0] * y[0] / S[0];
      states_[i].S[1] = hs[1] + K[1] * y[0] / S[0];

      // 7. update estimate covariance
      //  - P = (I - K*H)*P
      I_KH[0] = S[0] - K[0];
      I_KH[1] = 0;
      I_KH[2] = -K[1];
      I_KH[3] = S[0];

      simpleMatmul<T, 2, 2, 2, 2>(states_[i].P.data(), I_KH.data(), hP.data());
      states_[i].P = states_[i].P / S[0];

      // 9. return result
      ans[i] = states_[i].S[0];
    }
    return ans;
  }

 private:
  std::array<State, size> states_;

  template <class U, size_t R0, size_t C0, size_t R1, size_t C1>
  void simpleMatmul(U* ans, U lhs[R0 * C0], U rhs[R1 * C1],
                    bool trans = false) {
    // CAUTION! no access violation check
    static_assert(C0 == R1 || C0 == C1, "invalid mat shape for matmul");
    if constexpr (C0 != R1 && C0 == C1) trans = true;
    for (size_t i = 0; i < R0; i++)
      for (size_t j = 0; j < (trans ? R1 : C1); j++) {
        U sum = 0;
        for (size_t k = 0; k < C0; k++) {
          sum += lhs[i * C0 + k] * (trans ? rhs[j * R1 + k] : rhs[k * C1 + j]);
        }
        ans[i * (trans ? R1 : C1) + j] = sum;
      }
  }
};

#endif  // CLIM_KALMAN_FILTER_H_
