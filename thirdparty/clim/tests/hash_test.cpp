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
 * Description: hash test
 ****************************************/
#include "clim/hasher/hash.h"

#include <gtest/gtest.h>

TEST(Hash, City) { EXPECT_NE(CityHash64("a", 1), CityHash64("ab", 2)); }

TEST(Hash, Murmur3) {
  EXPECT_NE(MurmurHash3("a", 1, 0), MurmurHash3("a", 1, 1));
  EXPECT_NE(MurmurHash3("a", 1, 0), MurmurHash3("ab", 2, 0));
}
