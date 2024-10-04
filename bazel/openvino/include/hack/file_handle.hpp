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
#pragma once
#include <spdlog/sinks/stdout_color_sinks.h>
#include <spdlog/spdlog.h>

#include <fstream>
#include <sstream>

#include "clim/base64.h"
#include "clim/os.h"
#include "hack/json.hpp"

namespace hack {

class FileHandle {
 public:
  FileHandle() { init_state_ = Init(); }

  ~FileHandle() { Finish(); }

  nlohmann::json& json() & { return obj_; }

  std::stringstream& bin() & { return bytes_; }

  template <class... Args>
  void info(Args... args) {
    log_->info(std::forward<Args>(args)...);
  }

 private:
  bool Init() {
    log_ = spdlog::get("hack");
    if (!log_) {
      log_ = spdlog::create<spdlog::sinks::stdout_color_sink_mt>("hack");
    }
    if (!Environ().count("SUBWAY_KERNEL_DUMP") ||
        Environ()["SUBWAY_KERNEL_DUMP"].empty()) {
      log_->info(
          "GPU plugin kernel dump is disabled by environment "
          "SUBWAY_KERNEL_DUMP");
      return false;
    }
    int var = std::strtol(Environ()["SUBWAY_KERNEL_DUMP"].c_str(), nullptr, 10);
    if (var <= 0) {
      log_->info(
          "GPU plugin kernel dump is disabled by environment "
          "SUBWAY_KERNEL_DUMP");
      return false;
    }
    obj_["tag"] = "intel_gpu_kernel_dump";
    obj_["version"] = "2022.1.0";
    log_->info("Initialized gpu plugin kernel dump v{}",
               obj_["version"].get<std::string>());
    return true;
  }

  bool Finish() {
    if (!init_state_) {
      log_->info("GPU plugin kernel dump is disabled or not initialized.");
      return false;
    }

    auto str = bytes_.str();
    obj_["bin"] = base64::encode(str.c_str(), str.size());
    std::ofstream ofs;
    ofs.open("intel_gpu_kernel_dump.json");
    if (!ofs.is_open()) {
      log_->error("Failed to open {}", "intel_gpu_kernel_dump.json");
      return false;
    }
    ofs << obj_.dump(2) << std::endl;
    ofs.close();
    log_->info("Kernel dump finished into {}", "intel_gpu_kernel_dump.json");
    return true;
  }

  bool init_state_ = false;
  std::stringstream bytes_;
  nlohmann::json obj_;
  std::shared_ptr<spdlog::logger> log_;
};
}  // namespace hack
