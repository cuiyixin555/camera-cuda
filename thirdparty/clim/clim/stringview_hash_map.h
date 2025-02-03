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
 * Description: hashmap using std::string_view as key
 ****************************************/
#ifndef CLIM_STRINGVIEW_HASH_MAP_H_
#define CLIM_STRINGVIEW_HASH_MAP_H_
#include <string>
#include <string_view>
#include <unordered_map>
#include <unordered_set>
#include <utility>

/// NOLINTBEGIN(readability-identifier-naming)

/**
 * @brief A helper class to use std::string as hash map keys but also
 * accept std::string_view as api argument.
 *
 * @tparam T: hash map value type.
 * @tparam H: hasher type.
 */
template <class T, typename H = std::hash<std::string_view>>
class StringHashmap : public std::unordered_map<std::string, T, H> {
  using hash_map_t = std::unordered_map<std::string, T, H>;

 public:
  StringHashmap() : hash_map_t() {}
  ~StringHashmap() = default;

  template <class... Args>
  std::pair<typename hash_map_t::iterator, bool> try_emplace(
      std::string_view key, Args&&... args) {
    std::string k(key);
    return hash_map_t::try_emplace(k, std::forward<Args>(args)...);
  }

  typename hash_map_t::size_type erase(std::string_view key) noexcept {
    return hash_map_t::erase(std::string(key));
  }

  typename hash_map_t::size_type erase(const std::string& key) noexcept {
    return hash_map_t::erase(key);
  }

  typename hash_map_t::size_type erase(const char* key) noexcept {
    return hash_map_t::erase(key);
  }

  typename hash_map_t::size_type count(std::string_view key) const {
    return hash_map_t::count(std::string(key));
  }

  typename hash_map_t::size_type count(const std::string& key) const {
    return hash_map_t::count(key);
  }

  typename hash_map_t::size_type count(const char* key) const {
    return hash_map_t::count(key);
  }

  typename hash_map_t::mapped_type& at(std::string_view key) {
    return hash_map_t::at(std::string(key));
  }

  typename hash_map_t::mapped_type& at(const std::string& key) {
    return hash_map_t::at(key);
  }

  typename hash_map_t::mapped_type& at(const char* key) {
    return hash_map_t::at(key);
  }

  T& operator[](const std::string_view& key) {
    std::string k(key);
    return hash_map_t::operator[](k);
  }

  T& operator[](const std::string& key) { return hash_map_t::operator[](key); }

  T& operator[](const char* key) { return hash_map_t::operator[](key); }
};

/**
 * @brief A helper class to use std::string_view as hash map keys.
 *
 * @tparam T: hash map value type
 * @tparam H: hasher type
 */
template <class T, typename H = std::hash<std::string_view>>
class StringviewHashmap : public std::unordered_map<std::string_view, T, H> {
  using hash_map_t = std::unordered_map<std::string_view, T, H>;

 public:
  StringviewHashmap() : hash_map_t() {}
  ~StringviewHashmap() = default;

  void clear() noexcept {
    hash_map_t::clear();
    chars_.clear();
  }

  template <class... Args>
  std::pair<typename hash_map_t::iterator, bool> try_emplace(
      std::string_view key, Args&&... args) {
    std::string_view k(key);
    if (hash_map_t::find(key) == hash_map_t::end()) {
      chars_.insert(std::string(key));
      k = *(chars_.find(std::string(key)));
    }
    return hash_map_t::try_emplace(k, std::forward<Args>(args)...);
  }

  typename hash_map_t::size_type erase(const std::string_view& key) noexcept {
    hash_map_t::erase(key);
    return chars_.erase(std::string(key));
  }

  T& operator[](const std::string_view& key) {
    std::string_view k(key);
    if (hash_map_t::find(key) == hash_map_t::end()) {
      chars_.insert(std::string(key));
      k = *(chars_.find(std::string(key)));
    }
    return hash_map_t::operator[](k);
  }

 private:
  std::unordered_set<std::string> chars_;
};

/// NOLINTEND(readability-identifier-naming)
#endif  // CLIM_STRINGVIEW_HASH_MAP_H_
