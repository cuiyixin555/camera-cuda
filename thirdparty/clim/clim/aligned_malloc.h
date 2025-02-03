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
 * Description: Allocates memory on a specified alignment boundary.
 ****************************************/
#ifndef CLIM_ALIGNED_MALLOC_H_
#define CLIM_ALIGNED_MALLOC_H_
#include <cstdint>

#ifdef _WIN32
#include <Windows.h>

/**
 * @brief Allocates memory on a specified alignment boundary.
 *
 * @tparam T: pointer type returned for convenience
 * @param size: Size of the requested memory allocation.
 * @param page_size: The alignment value, which must be an integer power of 2.
 * @return T: A pointer to the memory block that was allocated or NULL if the
 *            operation failed. The pointer is a multiple of alignment.
 */
template <class T = uint8_t*>
inline T aligned_malloc(size_t size, size_t page_size = 0x1000) {  // NOLINT
  return reinterpret_cast<T>(_aligned_malloc(size, page_size));
}

/**
 * @brief Frees a block of memory that was allocated with aligned_malloc.
 *
 * @param p: A pointer to the memory block that was returned to the
 *           aligned_malloc.
 */
inline void aligned_free(void* p) { _aligned_free(p); }  // NOLINT
#else
#include <malloc.h>
#include <stdlib.h>

/**
 * @brief Allocates memory on a specified alignment boundary.
 *
 * @tparam T: pointer type returned for convenience
 * @param size: Size of the requested memory allocation.
 * @param page_size: The alignment value, which must be an integer power of 2.
 * @return T: A pointer to the memory block that was allocated or NULL if the
 *            operation failed. The pointer is a multiple of alignment.
 */
template <class T = uint8_t*>
inline T aligned_malloc(size_t size, size_t page_size = 0x1000) {  // NOLINT
  return reinterpret_cast<T>(memalign(page_size, size));
}

/**
 * @brief Frees a block of memory that was allocated with aligned_malloc.
 *
 * @param p: A pointer to the memory block that was returned to the
 *           aligned_malloc.
 */
inline void aligned_free(void* p) { free(p); }  // NOLINT
#endif
#endif  // CLIM_ALIGNED_MALLOC_H_
