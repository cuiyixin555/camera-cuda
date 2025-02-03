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
 * Description: basic reflection definition
 ****************************************/
#ifndef CLIM_REFLECT_H_
#define CLIM_REFLECT_H_
#include <string_view>

#ifdef _WIN32
#define OS_PATH_SLASH '\\'
#else
#define OS_PATH_SLASH '/'
#endif

/**
 * Get file basename without suffix.
 * FILESTEM("a/b/c.xxx") -> "c"
 *
 * TODO: Can someone find a better way than macro?
 */
#ifndef FILESTEM
#define FILESTEM(x)                                                        \
  std::string_view(x).substr(std::string_view(x).rfind(OS_PATH_SLASH) + 1, \
                             std::string_view(x).rfind('.') -              \
                                 std::string_view(x).rfind(OS_PATH_SLASH) - 1)
#endif

/**
 * Get file basename.
 * BASENAME("a/b/c/d/e.xxx") -> "e.xxx"
 */
#ifndef BASENAME
#define BASENAME(x) \
  std::string_view(x).substr(std::string_view(x).rfind(OS_PATH_SLASH) + 1)
#endif

#endif  // CLIM_REFLECT_H_
