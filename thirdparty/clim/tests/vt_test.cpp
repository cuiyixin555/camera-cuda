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
 * Description: unit test for vector tensor
 ****************************************/
#include "clim/vt/vt.h"

#include <gtest/gtest.h>

#include <vector>

using namespace vt;
using std::vector;

TEST(vttest_basic, PI) {
  vector<int> a{1, 2, 3, 4};
  vector<float> b{.1f, .2f, .3f};
  vector<char> c{'a', 'b', 'c'};
  EXPECT_EQ(PI(a), 24);
  EXPECT_FLOAT_EQ(PI(b), 0.006f);
  EXPECT_EQ(PI(c), 38);
  EXPECT_EQ(PI(a.begin(), a.end()), 24);
  EXPECT_FLOAT_EQ(PI(b.begin() + 1, b.end()), 0.06f);
}

TEST(vttest_basic, Op) {
  vector<int> a{1, 2, 3, 4};
  vector<float> b{.1f, .2f, -.3f};
  vector<uint8_t> c{1, 127, 128, 255};
  EXPECT_EQ(Add(a, a), (vector<int>{2, 4, 6, 8}));
  EXPECT_EQ(Sub(b, b), (vector<float>{0, 0, 0}));
  EXPECT_EQ(Add(c, c), (vector<uint8_t>{2, 254, 255, 255}));
  EXPECT_EQ(Add(a, 1), (vector<int>{2, 3, 4, 5}));
  EXPECT_EQ(Mul(b, 2.0f), (vector<float>{.2f, .4f, -.6f}));
  EXPECT_EQ(Div(a, 3), (vector<int>{0, 0, 1, 1}));
  EXPECT_EQ(Mod(c, (uint8_t)3), (vector<uint8_t>{1, 1, 2, 0}));
}

TEST(vttest_basic, ReduceOp) {
  vector<int> a{1, 2, 3, 4};
  vector<float> b{.1f, .2f, -.3f};
  vector<uint8_t> c{1, 127, 128, 255};
  EXPECT_EQ(ReduceMax(a), 4);
  EXPECT_EQ(ReduceMin(a), 1);
  EXPECT_EQ(ReduceSum(a), 10);
  EXPECT_EQ(ReduceMean(a), 2.5);
  EXPECT_EQ(ReduceMax(b), .2f);
  EXPECT_EQ(ReduceMin(b), -.3f);
  EXPECT_EQ(ReduceSum(b), 0);
  EXPECT_EQ(ReduceMean(b), 0);
}

TEST(vttest_basic, Index) {
  vector<int> shape{3, 4, 5};
  EXPECT_EQ(Index({0, 2, 3}, shape), 13);
  EXPECT_EQ(Index({1, 0, 0}, shape), 20);
  int arr2d[7][3]{
      1,  2,  3,  4,  5,  6,  7,  11, 12, 13, 14,
      15, 16, 17, 21, 22, 23, 24, 25, 26, 27,
  };
  int* arr = &arr2d[0][0];
  EXPECT_EQ(arr[Index<vector<int>>({2, 1}, {3, 7})], 22);
  shape = {7, 9, 11};
  for (int index = 0; index < PI(shape); index++) {
    EXPECT_EQ(Index(RevIndex(index, shape), shape), index);
  }
}

TEST(vttest_basic, Transpose) {
  vector<int> a{1, 2, 3, 4, 5, 6, 7, 8, 9};
  vector<int> as{3, 3};
  auto b = Transpose(a, as, {0, 1});
  for (int i = 0; i < a.size(); i++) {
    EXPECT_EQ(b[i], a[i]);
  }
  auto c = Transpose(a, as, {1, 0});
  for (int i = 0; i < a.size(); i++) {
    EXPECT_EQ(c[i], (vector<int>{1, 4, 7, 2, 5, 8, 3, 6, 9})[i]);
  }
  vector<int> x{41, 52, 72, 22, 22, 44, 68, 58, 78, 63, 85, 64,
                15, 7,  56, 20, 12, 35, 76, 33, 5,  35, 38, 89};
  vector<int> xs{1, 2, 3, 4};
  auto y = Transpose(x, xs, {3, 2, 1, 0});
  vector<int> g{41, 15, 22, 12, 78, 5,  52, 7,  44, 35, 63, 35,
                72, 56, 68, 76, 85, 38, 22, 20, 58, 33, 64, 89};
  for (int i = 0; i < g.size(); i++) {
    EXPECT_EQ(y[i], g[i]);
  }
  auto m = Arange<vector<int>>(0, 24);
  vector<int> ms{4, 3, 2};
  auto n = Transpose(m, ms, {2, 0, 1});
  vector<int> n_should_be{0, 2, 4, 6, 8, 10, 12, 14, 16, 18, 20, 22,
                          1, 3, 5, 7, 9, 11, 13, 15, 17, 19, 21, 23};
  for (int i = 0; i < n.size(); i++) {
    EXPECT_EQ(n[i], n_should_be[i]);
  }
}

TEST(vttest_basic, SliceAny) {
  vector<int> a{1, 2, 3, 4, 5, 6, 7, 8, 9};
  vector<int> as{3, 3}, b;
  EXPECT_NO_THROW(b = SliceAny(a, as, {-1, -1}));
  for (int i = 0; i < b.size(); i++) {
    EXPECT_EQ(b[i], a[i]);
  }
  EXPECT_NO_THROW(b = SliceAny(a, as, {-1, 0}));
  for (int i = 0; i < b.size(); i++) {
    EXPECT_EQ(b[i], (vector<int>{1, 4, 7})[i]);
  }
  EXPECT_NO_THROW(b = SliceAny(a, as, {1, -1}));
  for (int i = 0; i < b.size(); i++) {
    EXPECT_EQ(b[i], (vector<int>{4, 5, 6})[i]);
  }
}

TEST(vttest_basic, Concat) {
  vector<int> a = Arange<vector<int>>(1, 10);
  vector<int> b = Arange<vector<int>>(10, 19);
  vector<int> shape{1, 1, 3, 3};
  auto c = Concat(a, shape, b, shape, 0);
  for (int i = 0; i < a.size(); i++) {
    EXPECT_EQ(a[i], c[i]);
  }
  for (int i = 0; i < b.size(); i++) {
    EXPECT_EQ(b[i], c[i + a.size()]);
  }
  c = Concat(a, shape, b, shape, 1);
  for (int i = 0; i < a.size(); i++) {
    EXPECT_EQ(a[i], c[i]);
  }
  for (int i = 0; i < b.size(); i++) {
    EXPECT_EQ(b[i], c[i + a.size()]);
  }
  c = Concat(a, shape, b, shape, 2);
  for (int i = 0; i < a.size(); i++) {
    EXPECT_EQ(a[i], c[i]);
  }
  for (int i = 0; i < b.size(); i++) {
    EXPECT_EQ(b[i], c[i + a.size()]);
  }
  c = Concat(a, shape, b, shape, -1);
  for (int i = 0; i < c.size(); i++) {
    EXPECT_EQ(c[i], (vector<int>{1, 2, 3, 10, 11, 12, 4, 5, 6, 13, 14, 15, 7, 8,
                                 9, 16, 17, 18})[i]);
  }
  shape = {1, 3, 3, 1};
  c = Concat(a, shape, b, shape, -1);
  for (int i = 0; i < c.size(); i++) {
    EXPECT_EQ(c[i], (vector<int>{1, 10, 2, 11, 3, 12, 4, 13, 5, 14, 6, 15, 7,
                                 16, 8, 17, 9, 18})[i]);
  }
}

TEST(vttest_basic, Pad) {
  vector<int> a = Arange<vector<int>>(1, 17);
  vector<int> shape{4, 4};
  auto b = Pad(a, shape, {2, 1});
  EXPECT_EQ(b.size(), 4 * 7);
  auto b_col = SliceAny(b, vector<int>{4, 7}, {-1, 1});
  for (int i = 0; i < b_col.size(); i++) {
    EXPECT_EQ(b_col[i], 0);
  }
  auto c = Pad(a, shape, {1, 2, 0, 0});
  EXPECT_EQ(c.size(), 7 * 4);
  auto c_row = SliceAny(c, vector<int>{7, 4}, {6, -1});
  for (int i = 0; i < c_row.size(); i++) {
    EXPECT_EQ(c_row[i], 0);
  }
  auto d = Pad(a, shape, {1, 1, 1, 1});
  EXPECT_EQ(d.size(), 6 * 6);
}

TEST(vttest_basic, Hash) {
  vector<uint64_t> a{1, 2, 3, 4};
  vector<uint64_t> b{1, 2, 3, 4};
  vector<uint64_t> c{1, 2, 3, 4, 5};
  vector<uint64_t> d{2, 3, 4, 5};
  vector<int*> e{nullptr, nullptr, nullptr};
  EXPECT_EQ(VecHash(a), VecHash(b));
  EXPECT_NE(VecHash(a), VecHash(c));
  EXPECT_NE(VecHash(a), VecHash(d));
  EXPECT_NE(VecHash(a), VecHash(e));
  EXPECT_NE(VecHash(c), VecHash(d));
  EXPECT_NE(VecHash(c), VecHash(e));
  EXPECT_NE(VecHash(d), VecHash(e));
}

class PCrop : public testing::TestWithParam<std::tuple<int, int>> {
 public:
  void SetUp() override {
    auto par = GetParam();
    int dims = std::get<0>(par);
    int seed = std::get<1>(par);
    shape = RandomN<vector<int>>(vector<int>{dims}, 1, 32, seed);
    tensor = RandomN<vector<int>>(shape);
    for (int i = 0; i < dims; i++) crop.push_back(Random<int>(1, shape[i]));
  }

  void TearDown() override {
    tensor.clear();
    shape.clear();
    crop.clear();
  }

  vector<int> tensor;
  vector<int> shape;
  vector<int> crop;
};

TEST_P(PCrop, Crop) {
  vector<int> ans;
  EXPECT_NO_THROW(ans = Crop(tensor, shape, crop));
  EXPECT_EQ(ans.size(), PI(crop));
}

INSTANTIATE_TEST_SUITE_P(VTCrop, PCrop,
                         // first one is dimension, second one is random seeds
                         testing::Combine(testing::Values(1, 2, 3, 4, 5),
                                          testing::Values(1, 2, 3, 4)));
