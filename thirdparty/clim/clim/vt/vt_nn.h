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
 * Description: vector tensor for nerual network
 ****************************************/
#ifndef CLIM_VT_VT_NN_H_
#define CLIM_VT_VT_NN_H_
#include <iterator>

#include "vt.h"
namespace vt {
//! @addtogroup vt
//! @{

/**
 * @brief 2D convolution. Convolve a 3-D tensor `a` with weights `w`.
 * For more information please refer to:
 * https://pytorch.org/docs/stable/generated/torch.nn.Conv2d.html#torch.nn.Conv2d
 *
 * @tparam U: index type
 * @tparam T: data type
 * @param a: input tensor
 * @param as: input tensor shape, should be [IC, H, W]
 * @param w: weight
 * @param ws: weight shape, should be [OC, IC, K, K]
 * @param pad: paddings. Controls the amount of implicit zero-paddings on both
 * sides.
 * @param grs: groups. Controls the connections between inputs and outputs.
 * @param dila: dilations. Controls the spacing between the kernel points.
 * @param s: strides. Controls the stride for the cross-correlation.
 * @return convolutional result of shape [OC, (H-(K-1)D-1)/S+1,
 * (W-(K-1)D-1)/S+1]
 */
template <class Shape, class Container,
          must_be_int<typename Shape::value_type> = 0>
Container Conv(const Container& a, const Shape& as, const Container& w,
               const Shape& ws, typename Shape::value_type pad,
               typename Shape::value_type grs = 1,
               typename Shape::value_type dila = 1,
               typename Shape::value_type s = 1) {
  typedef typename Shape::value_type U;
  const U ic = as[0], oc = ws[0];
  if (ic / grs != ws[1] || oc % grs != 0) {
    printf("%s: Group mis-match!!\n", __FUNCTION__);
    return {};
  }
  if (a.size() != PI(as) || w.size() != PI(ws)) {
    printf("%s: Shape mis-match!!\n", __FUNCTION__);
    return {};
  }
  auto pa = vt::Pad(a, as, {pad, pad, pad, pad});
  Shape pas{ic, as[1] + pad * 2, as[2] + pad * 2};
  Shape bs{oc, (as[1] + pad * 2 - ((ws[2] - 1) * dila + 1) + 1) / s,
           (as[2] + pad * 2 - ((ws[3] - 1) * dila + 1) + 1) / s};
  Container b(vt::PI(bs), 0);
  for (U i = 0; i < b.size(); i++) {
    auto bi = vt::RevIndex(i, bs);
    U g = bi[0] / (oc / grs);
    for (U j = 0; j < ic / grs; j++) {
      for (U k1 = 0; k1 < ws[2]; k1++) {
        for (U k2 = 0; k2 < ws[3]; k2++) {
          auto pai = vt::Index(Shape{g * ws[1] + j, bi[1] * s + k1 * dila,
                                     bi[2] * s + k2 * dila},
                               pas);
          b[i] += pa[pai] * w[vt::Index({bi[0], j, k1, k2}, ws)];
        }
      }
    }
  }
  return b;
}

/**
 * @brief 2D transposed convolution.
 * For more information please refer to:
 * https://pytorch.org/docs/stable/generated/torch.nn.ConvTranspose2d.html#torch.nn.ConvTranspose2d
 *
 * @tparam U: shape type
 * @tparam T: data type
 * @param in: input tensor
 * @param is: input tensor shape, should be [IC, H, W]
 * @param wei: weights
 * @param ws: weight shape, should be [IC, OC, K, K]
 * @param s: strides. Controls the stride for the cross-correlation.
 * @param pad: paddings. Controls the amount of implicit zero-paddings on both
 * sides.
 * @param opad: output paddings. Controls the additional size added to one side
 * of the output shape.
 * @param grs: groups. Controls the connections between inputs and outputs.
 * @param dila: dilations. Controls the spacing between the kernel points.
 * @return convolutional result.
 */
template <class Shape, class Container,
          must_be_int<typename Shape::value_type> = 0>
Container Deconv(const Container& in, const Shape& is, const Container& wei,
                 const Shape& ws, int s = 2, int pad = 0, int opad = 0,
                 int grs = 1, int dila = 1) {
  typedef typename Shape::value_type U;
  const U kz = ws[2];        // kernel size
  const U ic = ws[0];        // input channel
  const U oc = ws[1] * grs;  // output channel
  const U ih = is[1];        // input height
  const U iw = is[2];        // input width
  const U oh = (ih - 1) * s - 2 * pad + dila * (kz - 1) + opad + 1;
  const U ow = (iw - 1) * s - 2 * pad + dila * (kz - 1) + opad + 1;
  const U ipad = dila * (kz - 1) - pad;
  if (ws[3] != kz || is[0] != ic || ic % grs != 0) {
    printf("[Deconv] Shape mis-match!!\n");
    return {};
  }
  if (opad >= s && opad >= dila) {
    printf("[Deconv] Output padding must < stride or dilation.\n");
    return {};
  }
  Shape os{oc, oh, ow};
  Container out(vt::PI(os), 0);
  for (int o = 0; o < oc; o++) {
    int g = o / ws[1];
    for (int i = 0; i < ic / grs; i++)
      for (int h = 0; h < ih; h++)
        for (int w = 0; w < iw; w++)
          for (int ki = 0; ki < kz; ki++)
            for (int kj = 0; kj < kz; kj++) {
              Shape oi = {o, h * s + ki * dila, w * s + kj * dila};
              oi[1] -= pad;
              oi[2] -= pad;
              if (oi[1] < 0 || oi[2] < 0) continue;
              if (oi[1] >= oh || oi[2] >= ow) continue;
              size_t oindex = vt::Index(oi, os);
              size_t iindex = vt::Index({i + g * ic / grs, h, w}, is);
              size_t windex =
                  vt::Index({i + g * ic / grs, o / grs, ki, kj}, ws);
              out[oindex] += wei[windex] * in[iindex];
            }
  }
  return out;
}

/**
 * @brief 2D max pooling
 * Applies a 2D max pooling over an input signal composed of several input
 * planes.
 *
 * @tparam U: shape type
 * @tparam T: data type
 * @param a: input tensor
 * @param as: input tensor shape
 * @param bz: block size, kernel size and stride are both equal to `bz`.
 * @return result tensor
 */
template <class Shape, class Container,
          must_be_int<typename Shape::value_type> = 0>
Container MaxPool(const Container& a, const Shape& as, int bz) {
  typedef typename Container::value_type T;
  Container b;
  Shape bs = as;
  bs[1] /= bz;
  bs[2] /= bz;
  b.resize(PI(bs));
  for (int i = 0; i < b.size(); i++) {
    T max = std::numeric_limits<T>::lowest();
    for (int j = 0; j < bz * bz; j++) {
      auto bi = RevIndex(i, bs);
      bi[1] = bi[1] * bz + j / bz;
      bi[2] = bi[2] * bz + j % bz;
      max = std::max(max, a[Index(bi, as)]);
    }
    b[i] = max;
  }
  return b;
}

/**
 * @brief 2D average pooling
 * Applies a 2D average pooling over an input signal composed of several input
 * planes.
 *
 * @tparam U: shape type
 * @tparam T: data type
 * @param a: input tensor
 * @param as: input tensor shape
 * @param bz: block size, kernel size and stride are both equal to `bz`.
 * @return result tensor
 */
template <class Shape, class Container,
          must_be_int<typename Shape::value_type> = 0>
Container AvgPool(const Container& a, const Shape& as, int bz) {
  typedef typename Container::value_type T;
  Container b;
  Shape bs = as;
  bs[1] /= bz;
  bs[2] /= bz;
  b.resize(vt::PI(bs));
  for (int i = 0; i < b.size(); i++) {
    T sum = T(0);
    for (int j = 0; j < bz * bz; j++) {
      auto bi = RevIndex(i, bs);
      bi[1] = bi[1] * bz + j / bz;
      bi[2] = bi[2] * bz + j % bz;
      sum += a[Index(bi, as)];
    }
    b[i] = sum / (bz * bz);
  }
  return b;
}

/**
 * @brief Add between two tensors.
 * The operation supports NumPy Broadcasting Semantics.
 *
 * @tparam U: shape type
 * @tparam T: data type
 * @param a: input tensor a
 * @param as: input tensor shape of a
 * @param b: input tensor b
 * @param bs: input tensor shape of b
 * @return a+b
 */
template <class Shape, class Container,
          must_be_int<typename Shape::value_type> = 0>
Container BroadcastAdd(const Container& a, const Shape& as, const Container& b,
                       const Shape& bs) {
  typedef typename Shape::value_type U;
  if (PI(as) == PI(bs)) {
    return Add(a, b);
  }
  Container y;
  if (PI(bs) <= 1) {
    for (auto& x : a) {
      y.push_back(x + b[0]);
    }
    return y;
  }
  U as_walker = 1;
  for (auto& _as : as) {
    as_walker *= _as;
    if (as_walker == PI(bs)) {
      auto itr_a = a.begin();
      U bs_walker = PI(as) / as_walker;
      for (auto& _b : b) {
        std::generate_n(std::back_inserter(y), bs_walker,
                        [&]() { return *(itr_a++) + _b; });
      }
    }
  }
  if (y.empty()) {
    printf("%s: Shape mis-match!!\n", __FUNCTION__);
  }
  return y;
}

/**
 * @brief Multiply between two tensors.
 * The operation supports NumPy Broadcasting Semantics.
 *
 * @tparam U: shape type
 * @tparam T: data type
 * @param a: input tensor a
 * @param as: input tensor shape of a
 * @param b: input tensor b
 * @param bs: input tensor shape of b
 * @return a*b
 */
template <class Shape, class Container,
          must_be_int<typename Shape::value_type> = 0>
Container BroadcastMul(const Container& a, const Shape& as, const Container& b,
                       const Shape& bs) {
  typedef typename Shape::value_type U;
  if (PI(as) == PI(bs)) {
    return Mul(a, b);
  }
  Container y;
  if (PI(bs) <= 1) {
    for (auto& x : a) {
      y.push_back(x * b[0]);
    }
    return y;
  }
  U as_walker = 1;
  for (auto& _as : as) {
    as_walker *= _as;
    if (as_walker == PI(bs)) {
      auto itr_a = a.begin();
      U bs_walker = PI(as) / as_walker;
      for (auto& _b : b) {
        std::generate_n(std::back_inserter(y), bs_walker,
                        [&]() { return *(itr_a++) * _b; });
      }
    }
  }
  if (y.empty()) {
    printf("%s: Shape mis-match!!\n", __FUNCTION__);
  }
  return y;
}

/**
 * @brief ReLU activation.
 * Applies the rectified linear unit function element-wise:
 * >>> Relu(x) = x if x > 0 else 0
 *
 * @tparam T: data type
 * @param a: input tensor
 * @return activated tensor
 */
template <class Container>
Container Relu(const Container& a) {
  Container y;
  for (auto& _a : a) {
    y.push_back(_a > 0 ? _a : 0);
  }
  return y;
}

/**
 * @brief Sigmoid activation.
 * Applies the element-wise function:
 * >>> Sigmoid(x) = 1 / (1 + exp(-x))
 *
 * @tparam T: data type
 * @param a: input tensor
 * @return activated tensor
 */
template <class Container, must_be_float<typename Container::value_type> = 0>
Container Sigmoid(const Container& a) {
  typedef typename Container::value_type T;
  Container y;
  for (auto& _a : a) {
    double ans = 1.0 / (1.0 + exp(-_a));
    y.push_back(static_cast<T>(ans));
  }
  return y;
}

/**
 * @brief Tanh activation.
 * Applies the element-wise function:
 * >>> Tanh(x) = (exp(x) - exp(-x)) / (exp(x) + exp(-x))
 *
 * @tparam T: data type
 * @param a: input tensor
 * @return activated tensor
 */
template <class Container, must_be_float<typename Container::value_type> = 0>
Container Tanh(const Container& a) {
  typedef typename Container::value_type T;
  Container y;
  for (auto& _a : a) {
    double ans = (exp(2 * _a) - 1.0) / (exp(2 * _a) + 1.0);
    y.push_back(static_cast<T>(ans));
  }
  return y;
}
//! @}
}  // namespace vt
#endif  // CLIM_VT_VT_NN_H_
