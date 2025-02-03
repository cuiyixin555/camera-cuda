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
 * Description:
 ****************************************/
#include "clim/os.h"

#include <gtest/gtest.h>

#include <fstream>

#include "clim/os_path.h"
#include "clim/str_replace.h"
namespace fs = std::filesystem;

TEST(Os, GetEnv) {
  for (const auto& [k, v] : Environ()) {
    std::cout << k << "=" << v << std::endl;
  }
}

TEST(OsPath, Glob) {
  // make test file
  auto dir = fs::temp_directory_path() / "clim_test";
  fs::create_directory(dir);
  fs::create_directory(dir / "subdir");
  std::ofstream(dir / "a.foo");
  std::ofstream(dir / "subdir/b.foo");
  std::ofstream(dir / "c.bar");
  std::ofstream(dir / "subdir/d.bar");
  std::ofstream(dir / "e.foobar");
  std::ofstream(dir / "subdir/h");
  auto ans = Glob(dir / "*");
  EXPECT_EQ(ans.size(), 3);
  ans = Glob(dir / "*", true);
  EXPECT_EQ(ans.size(), 6);
  ans = Glob(dir / "*.foo", true);
  EXPECT_EQ(ans.size(), 2);
  ans = Glob(dir / "*.b??");
  EXPECT_EQ(ans.size(), 1);
  ans = Glob(dir / "**/*.bar");
  EXPECT_EQ(ans.size(), 2);
  fs::remove_all(dir);
}

TEST(OsPath, GetCurrentModuleDir) {
  // Get current UT directory from GetCurrentModuleDir
  auto _module_dir = GetCurrentModuleDir();
  auto module_dir = StrReplace(_module_dir, "\\", "/");
  // Get current UT directory from environment
  auto env_dir = Environ()["RUNFILES_DIR"];
  env_dir = env_dir + "/clim/tests/";
  // Compare
  EXPECT_NE(env_dir == module_dir, 0);
}
