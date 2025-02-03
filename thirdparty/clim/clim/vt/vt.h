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
 * Description: tensor shape on std::vector
 ****************************************/
#ifndef CLIM_VT_VT_H_
#define CLIM_VT_VT_H_
#include <algorithm>
#include <cmath>
#include <cstring>
#include <functional>
#include <iterator>
#include <limits>
#include <random>
#include <stdexcept>
#include <type_traits>
#include <utility>

/**
 * @defgroup vt
 * a vector tensor lib
 *
 * Implements a lot of useful tensor operation such as vectorize add/sub/mul/div
 * or slice/crop/reduce to stl container or class implements iterator interface.
 * @ingroup util
 */

namespace vt {
//! @addtogroup vt
//! @{

template <class T>
using must_be_int = std::enable_if_t<std::is_integral<T>::value, int>;
template <class T>
using must_be_float = std::enable_if_t<std::is_floating_point<T>::value, int>;
template <class T>
using must_be_pointer = std::enable_if_t<std::is_pointer<T>::value, int>;
template <class T>
using must_not_be_pointer = std::enable_if_t<!std::is_pointer<T>::value, int>;

/**
 * @brief \Pi_{i=beg}^{end}(i)
 * A cumulative product from `beg` to `end`
 *
 * @tparam U: data type
 * @param beg: start number
 * @param end: stop number
 * @return \Pi_{i=beg}^{end}(i)
 */
template <class U>
auto PI(U beg, U end) {
  using T = typename std::iterator_traits<U>::value_type;
  auto n = std::distance(beg, end);
  if (n == 0) return T(0);
  T prod = T(1);
  for (U i = beg; i != end; ++i) {
    prod *= *i;
  }
  return prod;
}

/**
 * @brief \Pi(a_i)
 * A cumulative product of STL container `a`.
 *
 * @tparam T: data type
 * @param a: input vector
 * @return T: \Pi(a_i)
 */
template <class Container>
typename Container::value_type PI(const Container& a) {
  return PI(a.begin(), a.end());
}

/**
 * @brief Absolute value of STL container `a`.
 *
 * @tparam T: data type
 * @param a: input vector
 * @return T: abs(a_i)
 */
template <class Container>
Container Abs(const Container& a) {
  Container abs_a = a;
  for (auto& v : abs_a) {
    v = std::abs(v);
  }
  return abs_a;
}

/**
 * @brief A binocular operator between `a` and `b`.
 *
 * @tparam T: STL container
 * @tparam Fn: op type
 * @param a: input a
 * @param b: input b
 * @param op: operation
 * @return a op b
 */
template <class Container, class Fn>
Container Op(const Container& a, const Container& b, Fn op) {
  using T = typename Container::value_type;
  using D = typename Container::difference_type;
  auto size_a = std::distance(a.begin(), a.end());
  auto size_b = std::distance(b.begin(), b.end());
  if (size_a != size_b) {
    throw std::invalid_argument("input a and input b has different size");
  }
  Container ret;
  for (D i = 0; i < size_a; i++) {
    auto tmp = op(a[i], b[i]);
    if (tmp > std::numeric_limits<T>::max()) {
      tmp = std::numeric_limits<T>::max();
    } else if (tmp < std::numeric_limits<T>::lowest()) {
      tmp = std::numeric_limits<T>::lowest();
    }
    ret.push_back(static_cast<T>(tmp));
  }
  return ret;
}

/**
 * @brief A binocular operator between `a` and scalar `b`.
 *
 * @tparam T: data type
 * @tparam Fn: op type
 * @param a: input a
 * @param b: input b
 * @param op: operation
 * @return a op b
 */
template <class Container, class Fn>
Container Op(const Container& a, typename Container::value_type b, Fn op) {
  using T = typename Container::value_type;
  using D = typename Container::difference_type;
  auto size_a = std::distance(a.begin(), a.end());
  Container ret;
  for (D i = 0; i < size_a; i++) {
    auto tmp = op(a[i], b);
    if (tmp > std::numeric_limits<T>::max()) {
      tmp = std::numeric_limits<T>::max();
    } else if (tmp < std::numeric_limits<T>::lowest()) {
      tmp = std::numeric_limits<T>::lowest();
    }
    ret.push_back(static_cast<T>(tmp));
  }
  return std::move(ret);
}

/** @brief vectorize add */
template <class Container, class U>
Container Add(const Container& a, U b) {
  using T = typename Container::value_type;
  return Op(a, b, [](T x, T y) { return x + y; });
}

/** @brief vectorize substrate */
template <class Container, class U>
Container Sub(const Container& a, const U b) {
  using T = typename Container::value_type;
  return Op(a, b, [](T x, T y) { return x - y; });
}

/** @brief vectorize multiply */
template <class Container, class U>
Container Mul(const Container& a, U b) {
  using T = typename Container::value_type;
  return Op(a, b, [](T x, T y) { return x * y; });
}

/** @brief vectorize divide */
template <class Container, class U>
Container Div(const Container& a, U b) {
  using T = typename Container::value_type;
  return Op(a, b, [](T x, T y) { return x / y; });
}

/** @brief vectorize modular */
template <class Container, class U,
          must_be_int<typename Container::value_type> = 0>
Container Mod(const Container& a, U b) {
  using T = typename Container::value_type;
  return Op(a, b, [](T x, T y) { return x % y; });
}

/** @brief all element equality */
template <class Container>
inline bool operator==(const Container& a, const Container& b) {
  using D = typename Container::difference_type;
  auto size_a = std::distance(a.begin(), a.end());
  auto size_b = std::distance(b.begin(), b.end());
  if (size_a != size_b) {
    return false;
  }
  for (D i = 0; i < size_a; i++) {
    if (*(a.begin() + i) != *(b.begin() + i)) {
      return false;
    }
  }
  return true;
}

/** @brief reduced sum of vector */
template <class Container>
typename Container::value_type ReduceSum(const Container& a) {
  using T = typename Container::value_type;
  T s = T(0);
  for (auto& v : a) {
    s += v;
  }
  return s;
}

/** @brief reduced average of vector */
template <class Container, class U = double>
U ReduceMean(const Container& a) {
  U s = static_cast<U>(ReduceSum(a));
  auto size_a = std::distance(a.begin(), a.end());
  return static_cast<U>(s / static_cast<U>(size_a));
}

/** @brief reduced variation of vector */
template <class Container, class U = double>
U ReduceVar(const Container& a) {
  auto size_a = std::distance(a.begin(), a.end());
  U mean = ReduceMean<Container, U>(a);
  U var = 0;
  for (auto& v : a) {
    var += (static_cast<U>(v) - mean) * (static_cast<U>(v) - mean);
  }
  return var / static_cast<U>(size_a);
}

/** @brief reduced maximal value of vector */
template <class Container>
typename Container::value_type ReduceMax(const Container& a) {
  using T = typename Container::value_type;
  T max = std::numeric_limits<T>::lowest();
  for (auto& v : a) {
    max = std::max(v, max);
  }
  return max;
}

/** @brief reduced minimal value of vector */
template <class Container>
typename Container::value_type ReduceMin(const Container& a) {
  using T = typename Container::value_type;
  T min = std::numeric_limits<T>::max();
  for (auto& v : a) {
    min = std::min(v, min);
  }
  return min;
}

/**
 * @brief Peek signal-noise ratio between `a` and `b`
 *
 * @param a: input a
 * @param b: input b
 * @param max_val: the maximal value of the definition
 * @return PSNR
 */
template <class Container, typename U = double>
U PSNR(const Container& a, const Container& b,
       typename Container::value_type max_val =
           std::numeric_limits<typename Container::value_type>::max()) {
  auto size_a = std::distance(a.begin(), a.end());
  auto size_b = std::distance(b.begin(), b.end());
  if (size_a != size_b) {
    throw std::invalid_argument("input a and input b have different size");
  }
  auto err = Sub(a, b);
  auto err2 = Mul(err, err);
  float mse = ReduceSum(err2) / static_cast<U>(size_a);
  float max = static_cast<U>(max_val);
  return static_cast<U>(U(10) * log10(max * max / mse));
}

/**
 * @brief Same as python `range`
 *
 * @tparam T: return data type
 * @tparam U: step type
 * @param start: the starting value for the set of points
 * @param stop: the ending value for the set of points
 * @param step: the gap between each pair of adjacent points
 * @return a 1-D tensor of size `(stop-start)/step` with values from the
 *     interval ``[start, end)`` taken with common difference `step` beginning
 *     from `start`.
 */
template <class Container, class U, must_be_int<U> = 0>
Container Arange(U start, U stop, U step = 1) {
  Container r;
  using T = typename Container::value_type;
  for (U i = start; i < stop; i += step) {
    r.push_back(static_cast<T>(i));
  }
  return r;
}

/**
 * @brief Get the flatten index from N-D index `i` of shape `s`
 * This is the reverse function of `RevIndex`.
 *
 * @tparam Shape: STL container representing shape
 * @param i: an index for N-D tensor
 * @param s: the shape of the tensor
 * @return the flatten(converted) index.
 */
template <class Shape, must_be_int<typename Shape::value_type> = 0>
typename Shape::value_type Index(const Shape& i, const Shape& s) {
  using D = typename Shape::difference_type;
  auto size_i = std::distance(i.begin(), i.end());
  auto size_s = std::distance(s.begin(), s.end());
  if (size_i != size_s) {
    throw std::invalid_argument("index and shape have different dimension");
  }
  using T = typename Shape::value_type;
  T r(0);
  for (D x = 0; x < size_i; x++) {
    if (x + 1 == size_s) {
      r += *(i.begin() + x);
    } else {
      r += *(i.begin() + x) * PI(s.begin() + x + 1, s.end());
    }
  }
  return r;
}

/**
 * @brief Get the N-D index from the flatten index `i` of shape `s`.
 * This is the reverse function of `Index`.
 *
 * @tparam Shape: STL container representing shape
 * @param i: flatten index
 * @param s: tensor shape
 * @return a 1-D vector of N-D index
 */
template <class Shape, must_be_int<typename Shape::value_type> = 0>
Shape RevIndex(typename Shape::value_type i, const Shape& s) {
  Shape r;
  for (auto v = s.rbegin(); v != s.rend(); ++v) {
    r.push_back(i % *v);
    i /= *v;
  }
  return Shape(r.rbegin(), r.rend());
}

/**
 * @brief Access a tensor via a slice index.
 * Same as the slice index of python list.
 *
 * @tparam U: index type
 * @tparam T: data type
 * @param a: input tensor
 * @param s: input tensor shape
 * @param ai: slice index, use -1 to slice all elements.
 * @return sliced tensor
 */
template <class Shape, class Container,
          must_be_int<typename Shape::value_type> = 0>
Container SliceAny(const Container& a, const Shape& s, const Shape& ai) {
  using U = typename Shape::value_type;
  using D = typename Shape::difference_type;
  Container b;  // sliced vector
  Shape bs;     // sliced shape
  auto dim = std::distance(s.begin(), s.end());
  auto size_a = std::distance(a.begin(), a.end());
  Shape select;
  Shape axis;
  if (size_a != PI(s)) {
    throw std::invalid_argument("Shape mis-match!!");
  }
  if (std::distance(ai.begin(), ai.end()) != dim) {
    throw std::invalid_argument("Index and slice doesn't match input shape!!");
  }
  for (D i = 0; i < dim; i++) {
    if (ai[i] >= s[i] && ai[i] != U(-1)) {
      throw std::logic_error("Axis overflow!!");
    }
    if (ai[i] == U(-1)) {
      bs.emplace_back(s[i]);
      axis.emplace_back(static_cast<U>(i));
      select.push_back(0);
    } else {
      select.push_back(ai[i]);
    }
  }
  for (U i = 0; i < PI(bs); i++) {
    auto bi = RevIndex(i, bs);
    for (typename Shape::size_type j = 0; j < axis.size(); j++) {
      select[axis[j]] = bi[j];
    }
    b.push_back(a[Index(select, s)]);
  }
  return b;
}

/**
 * @brief Cast a Container to Target with another value type
 */
template <class Target, class Container>
Target Cast(const Container& a) {
  using U = typename Target::value_type;
  Target b;
  auto size_a = std::distance(a.begin(), a.end());
  auto iter = a.begin();
  std::generate_n(std::back_inserter(b), size_a,
                  [&]() { return static_cast<U>(*iter++); });
  return b;
}

/**
 * @brief Crop a tensor `a`.
 *
 * @tparam Container: tensor STL container
 * @tparam Shape: STL container representing shape
 * @param a: tensor to be cropped
 * @param as: tensor shape
 * @param offset: cropped offset
 * @param crop: cropped shape
 * @return Container: cropped tensor
 */
template <class Container, class Shape,
          must_be_int<typename Shape::value_type> = 0>
Container Crop(const Container& a, const Shape& as, Shape offset, Shape crop) {
  using U = typename Shape::value_type;
  Container b;
  U size_a = std::distance(a.begin(), a.end());
  if (size_a != PI(as)) {
    throw std::invalid_argument("Input size and shape mismatch!!");
  }
  auto dim = std::distance(as.begin(), as.end());
  auto dim_o = std::distance(offset.begin(), offset.end());
  auto dim_c = std::distance(crop.begin(), crop.end());
  if (dim_o != dim_c || dim_o > dim || dim_c > dim) {
    throw std::invalid_argument("offset and crop dimension mismatch!!");
  }
  // if crop dimension is less than tensor shape, padding it with 1
  // (same as python slice)
  std::fill_n(std::back_inserter(offset), dim - dim_o, 0);
  std::fill_n(std::back_inserter(crop), dim - dim_c, U(1));
  for (U i = 0; i < PI(crop); i++) {
    auto crop_index = Add(RevIndex(i, crop), offset);
    b.push_back(a[Index(crop_index, as)]);
  }
  return b;
}

/**
 * @brief Crop a tensor `a`.
 *
 * @tparam Container: tensor STL container
 * @tparam Shape: STL container representing shape
 * @param a: tensor to be cropped
 * @param as: tensor shape
 * @param crop: cropped shape
 * @return Container: cropped tensor
 */
template <class Container, class Shape,
          must_be_int<typename Shape::value_type> = 0>
Container Crop(const Container& a, const Shape& as, const Shape& crop) {
  Shape offset = crop;
  for (auto& x : offset) x = 0;
  return Crop(a, as, offset, crop);
}

/**
 * @brief Transpose vector among axises.
 *
 * @tparam U: shape type
 * @tparam T: data type
 * @param a: input tensor
 * @param s: input tensor shape
 * @param perm: index permutation order
 * @return transposed tensor
 */
template <class Shape, class Container,
          must_be_int<typename Shape::value_type> = 0>
Container Transpose(const Container& a, const Shape& s, const Shape& perm) {
  using D = typename Shape::difference_type;
  using U = typename Shape::value_type;
  Container b;
  Shape ns = s;
  auto size_p = std::distance(perm.begin(), perm.end());
  auto dim = std::distance(s.begin(), s.end());
  if (size_p != dim) {
    throw std::invalid_argument("Permutation mis-matches shape!!");
  }
  for (D i = 0; i < dim; i++) {
    if (perm[i] >= U(dim)) {
      throw std::logic_error("Permutation exceeds shape index!!");
    }
    ns[i] = s[perm[i]];
  }
  auto size_a = std::distance(a.begin(), a.end());
  for (U i = 0; i < static_cast<U>(size_a); i++) {
    auto bi = RevIndex(i, ns);
    auto ai = bi;
    for (int j = 0; j < ns.size(); j++) {
      ai[perm[j]] = bi[j];
    }
    b.push_back(a[Index(ai, s)]);
  }
  return b;
}

/**
 * @brief Concatenates the tensor `a` and `b` in the given dimension.
 * All tensors must either have the same shape (except in the concatenating
 * dimension) or be empty.
 *
 * @tparam U: shape type
 * @tparam T: data type
 * @param a: input tensor a
 * @param as: input tensor a's shape
 * @param b: input tensor b
 * @param bs: input tensor b's shape
 * @param axis: the dimension over which the tensors are concatenated
 * @return output tensor
 */
template <class Shape, class Container,
          must_be_int<typename Shape::value_type> = 0>
Container Concat(const Container& a, const Shape& as, const Container& b,
                 const Shape& bs, int axis = -1) {
  using U = typename Shape::value_type;
  using T = typename Container::value_type;
  Container c;
  auto dim = std::distance(as.begin(), as.end());
  auto size_a = std::distance(a.begin(), a.end());
  auto size_b = std::distance(b.begin(), b.end());
  if (size_a != PI(as) || size_b != PI(bs) ||
      dim != std::distance(bs.begin(), bs.end())) {
    throw std::invalid_argument("Shape mis-match!!");
  }
  if (axis < 0) {
    axis += static_cast<int>(dim);
  }
  if (axis < 0 || axis >= dim) {
    throw std::invalid_argument("Invalid axis!!");
  }
  for (int i = 0; i < dim; i++) {
    if (i == axis) continue;
    if (as[i] != bs[i]) {
      throw std::logic_error("Shape mis-match!!");
    }
  }
  c.resize(PI(as) + PI(bs));
  auto p = Arange<Shape, U>(0, static_cast<U>(dim));
  p.erase(p.begin() + axis);
  p.insert(p.begin(), axis);
  auto t_a = Transpose(a, as, p);
  auto t_b = Transpose(b, bs, p);
  memcpy(c.data(), t_a.data(), t_a.size() * sizeof(T));
  memcpy(c.data() + t_a.size(), t_b.data(), t_b.size() * sizeof(T));
  auto cs = as;
  cs.erase(cs.begin() + axis);
  cs.insert(cs.begin(), 0);
  cs[0] = as[axis] + bs[axis];
  auto rp = Arange<Shape, U>(1, U(as.size()) + 1);
  rp.insert(rp.begin() + axis, 0);
  rp.erase(rp.end() - 1);
  c = Transpose(c, cs, rp);
  return c;
}

/**
 * @brief Pads tensor `a`.
 *
 * @tparam U: shape type
 * @tparam T: data type
 * @param a: input tensor a
 * @param s: input tensor shape
 * @param pad: m-elements tuple, where m/2 <= input dimensions and m is even.
 * @return padded tensor
 */
template <class Shape, class Container,
          must_be_int<typename Shape::value_type> = 0>
Container Pad(const Container& a, const Shape& s, Shape pad) {
  Container b;
  auto size_pad = std::distance(pad.begin(), pad.end());
  if (size_pad % 2) {
    throw std::invalid_argument("pad list must be multiple of 2!!");
  }
  auto dim = std::distance(s.begin(), s.end());
  for (auto i = dim * 2 - size_pad; i > 0; i--) {
    pad.insert(pad.begin(), 0);
  }
  Shape ns = s;
  for (int i = 0; i < dim; i++) {
    ns[i] += pad[i * 2] + pad[i * 2 + 1];
  }
  for (int i = 0; i < PI(ns); i++) {
    auto sb = RevIndex(i, ns);
    bool inbound = true;
    for (int j = 0; j < dim; j++) {
      if (sb[j] < pad[j * 2] || sb[j] >= s[j] + pad[j * 2]) {
        inbound = false;
        break;
      }
    }
    if (inbound) {
      auto sa = sb;
      for (int j = 0; j < dim; j++) {
        sa[j] -= pad[j * 2];
      }
      b.push_back(a[Index(sa, s)]);
    } else {
      b.push_back(0);
    }
  }
  return b;
}

/**
 * @brief Generate random numbers
 * @param seed: seed == 0 -> generate random data every time.
 *              seed != 0 -> fix seed and fix pseudo random number.
 */
template <class Container>
Container RandomN(
    size_t size,
    typename Container::value_type min =
        std::numeric_limits<typename Container::value_type>::min(),
    typename Container::value_type max =
        std::numeric_limits<typename Container::value_type>::max(),
    uint32_t seed = 1) {
  using T = typename Container::value_type;
  std::random_device rd;
  std::mt19937 gen(seed ? seed : rd());
  std::uniform_real_distribution<> dis(static_cast<double>(min),
                                       static_cast<double>(max));
  Container a;
  for (size_t i = 0; i < size; i++) {
    a.push_back(static_cast<T>(dis(gen)));
  }
  return a;
}

/**
 * @brief Generate random numbers
 * @param seed: seed == 0 -> generate random data every time.
 *              seed != 0 -> fix seed and fix pseudo random number.
 */
template <class Container, class Shape,
          must_be_int<typename Shape::value_type> = 0>
Container RandomN(
    const Shape& shape,
    typename Container::value_type min =
        std::numeric_limits<typename Container::value_type>::min(),
    typename Container::value_type max =
        std::numeric_limits<typename Container::value_type>::max(),
    uint32_t seed = 1) {
  return RandomN<Container>(PI(shape), min, max, seed);
}

/** Generate a single random uniform distributed number. */
template <class T>
T Random(T min = std::numeric_limits<T>::min(),
         T max = std::numeric_limits<T>::max(), uint32_t seed = 0) {
  std::random_device rd;
  std::mt19937 gen(seed ? seed : rd());
  std::uniform_real_distribution<> dis(min, max);
  return static_cast<T>(dis(gen));
}

/**
 * @brief Hashing an stl container holding any kind of pointer type
 *
 * @param v: the container to be hashed.
 * @return hash size
 */
template <class Container, must_be_pointer<typename Container::value_type> = 0>
uint64_t VecHash(const Container& v) {
  constexpr uint64_t kPrime = 0x00000100000001b3;
  uint64_t seed = 0xCCCCCCCCCCCCCCCC;  // NOLINT(*-magic-numbers)
  std::hash<std::uintptr_t> hasher{};
  for (auto x : v) {
    seed ^= hasher(reinterpret_cast<std::uintptr_t>(x));
    seed *= kPrime;
  }
  return seed;
}

/**
 * @brief Hashing an stl container holding any kind of non-pointer type
 *
 * @param v: the container to be hashed.
 * @return hash size
 */
template <class Container,
          must_not_be_pointer<typename Container::value_type> = 0>
uint64_t VecHash(const Container& v) {
  constexpr uint64_t kPrime = 0x00000100000001b3;
  uint64_t seed = 0xCCCCCCCCCCCCCCCC;  // NOLINT(*-magic-numbers)
  std::hash<typename Container::value_type> hasher{};
  for (auto x : v) {
    seed ^= hasher(x);
    seed *= kPrime;
  }
  return seed;
}
//! @}
}  // namespace vt
#endif  // CLIM_VT_VT_H_
