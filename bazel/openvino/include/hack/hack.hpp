#pragma once
#include <algorithm>
#include <mutex>
#include <set>
#include <thread>
#include <vector>

#include "hack/file_handle.hpp"

namespace hack {
template <typename T>
// NOLINTNEXTLINE
inline bool unions(const std::vector<T>& lhs, const std::vector<T>& rhs) {
  std::set<T> s0(lhs.begin(), lhs.end());
  std::set<T> s1(rhs.begin(), rhs.end());
  for (auto& x : s0) {
    if (s1.count(x)) return true;
  }
  return false;
}
}  // namespace hack
