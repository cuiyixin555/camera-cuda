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
 * Description: os utilities
 ****************************************/
#ifndef CLIM_OS_H_
#define CLIM_OS_H_
#include <map>
#include <string>

/**
 * @brief Get all system environment variables
 *
 * @return std::map<std::string, std::string>
 */
static std::map<std::string, std::string>& Environ();

#ifdef _WIN32
#include <Windows.h>

static std::map<std::string, std::string>& Environ() {
  static std::map<std::string, std::string> ret;
  if (!ret.empty()) return ret;

  auto* envs = GetEnvironmentStrings();
  std::string k, v;  // key, value
  bool eos = false;  // end of string
  bool eok = false;
  for (int i = 0;; i++) {
    // end of whole string
    if (envs[i] == '\0' && eos) {
      break;
    } else {
      eos = false;
    }
    if (envs[i] == '\0') {
      eos = true;
      eok = false;
      if (!k.empty()) ret[k] = v;
      k.clear();
      v.clear();
    } else if (envs[i] == '=') {
      eok = true;
    } else if (eok) {
      v.push_back(static_cast<const char>(envs[i]));
    } else {
      k.push_back(static_cast<const char>(envs[i]));
    }
  }
  FreeEnvironmentStrings(envs);
  return ret;
}
#else
#include <unistd.h>

extern char** environ;  // NOLINT

static std::map<std::string, std::string>& Environ() {
  static std::map<std::string, std::string> ret;
  if (!ret.empty()) return ret;

  const char* envs = environ[0];
  std::string k, v;  // key, value
  bool eos = false;  // end of string
  bool eok = false;
  for (int i = 0;; i++) {
    // end of whole string
    if (envs[i] == '\0' && eos) {
      break;
    } else {
      eos = false;
    }
    if (envs[i] == '\0') {
      eos = true;
      eok = false;
      if (!k.empty()) ret[k] = v;
      k.clear();
      v.clear();
    } else if (envs[i] == '=') {
      eok = true;
    } else if (eok) {
      v.push_back(static_cast<const char>(envs[i]));
    } else {
      k.push_back(static_cast<const char>(envs[i]));
    }
  }
  return ret;
}
#endif
#endif  // CLIM_OS_H_
