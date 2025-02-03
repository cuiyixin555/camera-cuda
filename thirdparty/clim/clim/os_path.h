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
 * Description: Query OS/User path
 ****************************************/
#ifndef CLIM_OS_PATH_H_
#define CLIM_OS_PATH_H_
#include <fstream>
#include <regex>
#include <string>
#include <utility>
#include <vector>

#ifdef _WIN32
#include <filesystem>
#else
#ifdef HAS_STD_FS
#include <filesystem>
#endif
#ifdef HAS_STD_EXP_FS
#include <experimental/filesystem>
namespace std {
namespace filesystem = experimental::filesystem;
}  // namespace std
#endif
#endif

/** @ingroup util
 * @brief Get the current working directory
 *
 * @return absolute path of the directory
 */
inline std::string GetCurrentModuleDir();

/** @ingroup util
 * @brief Read and return file binary bytes
 *
 * @tparam T : type of the bytes container
 * @param path : file url
 * @return std::vector<T>.
 */
template <class T = char>
std::vector<T> ReadBytes(const std::string& path) {
  std::vector<T> blobs;
  size_t size = 0;
  std::ifstream ifs(path, std::ios::binary);
  if (ifs.is_open()) {
    ifs.seekg(0, std::ios::end);
    size = ifs.tellg();
    ifs.seekg(0, std::ios::beg);
    blobs.resize(size / sizeof(T));
    ifs.read(reinterpret_cast<char*>(blobs.data()), size);
  }
  return std::move(blobs);
}

/**
 * @brief Glob a file pattern.
 *
 * @param path: unix-style path pattern.
 * @param recursive: whether to glob recursively.
 * @return a list of matched files.
 *
 * @example
 * Glob("/tmp/a/ *.txt");  // [tmp/a/1.txt, tmp/a/2.txt, ...]
 * Glob("/tmp/ ** / *.cpp");  // equivalent to Glob("/tmp/ *.cpp", true);
 */
inline std::vector<std::filesystem::path> Glob(
    const std::filesystem::path& path, bool recursive = false) {
  // change unix-style wildcard to regular expression.
  auto unix_to_regex = [](const std::filesystem::path& path) {
    std::string re;
    for (auto c : path.string()) {
      if (c == '?') {
        re += '.';
      } else if (c == '*') {
        re += '.';
        re += c;
      } else if (c == '.') {
        re += '\\';
        re += c;
      } else {
        re += c;
      }
    }
    return re;
  };

  std::vector<std::filesystem::path> matched;
  std::filesystem::path glob_pattern("*"), stem, dir;
  if (path.has_filename()) glob_pattern = path.filename();
  stem = glob_pattern.stem().empty() ? "*" : glob_pattern.stem();
  stem += glob_pattern.extension();
  dir = path.parent_path();
  if (recursive) {
    for (const auto& p : std::filesystem::recursive_directory_iterator(dir)) {
      if (p.is_regular_file() &&
          std::regex_match(p.path().filename().string(),
                           std::regex(unix_to_regex(stem)))) {
        matched.push_back(p.path());
      }
    }
    return matched;
  }
  // seaching parent path for `**`, and search its parent path recursively.
  auto father = dir;
  while (father.has_parent_path() && father.has_relative_path()) {
    if (father.has_filename() && father.filename() == "**") {
      return Glob(father.parent_path() / (stem), true);
    }
    father = father.parent_path();
  }
  // normally search dir path.
  for (const auto& p : std::filesystem::directory_iterator(dir)) {
    if (p.is_regular_file() &&
        std::regex_match(p.path().filename().string(),
                         std::regex(unix_to_regex(stem)))) {
      matched.push_back(p.path());
    }
  }
  return matched;
}

#ifdef _WIN32
#include <Windows.h>
#include <intrin.h>
#include <shlwapi.h>

inline std::string GetCurrentModuleDir() {
  char bin_dir[256] = {0};  // NOLINT
  HMODULE caller_module = nullptr;
  void* caller_address = _ReturnAddress();
  // GetModuleHandleEx from return address
  if (!GetModuleHandleEx(GET_MODULE_HANDLE_EX_FLAG_FROM_ADDRESS,
                         (LPCTSTR)caller_address, &caller_module)) {
    return std::string("GetModuleHandleEx_failed");
  }
  // Get module file path
  if (GetModuleFileNameA(caller_module, bin_dir, sizeof(bin_dir)) == 0) {
    return std::string("GetModuleFileNameA_failed");
  }
  std::string cwd_path(bin_dir);
  size_t pos = cwd_path.find_last_of(L'\\') + 1;
  return std::string(cwd_path.cbegin(), cwd_path.cbegin() + pos);
}
#else
#include <unistd.h>

inline std::string GetCurrentModuleDir() {
  char sztmp[32]{};
  snprintf(sztmp, sizeof(sztmp), "/proc/%d/exe", getpid());
  char buf[1024]{};
  int bytes = readlink(sztmp, buf, sizeof(buf));
  buf[bytes] = '\0';
  std::string url(buf);
  size_t pos = url.find_last_of('/') + 1;
  return std::string(url.cbegin(), url.cbegin() + pos);
}
#endif
#endif  // CLIM_OS_PATH_H_
