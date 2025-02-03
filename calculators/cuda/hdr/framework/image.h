

#ifndef INCLUDED_IMAGE
#define INCLUDED_IMAGE

#pragma once

#include <algorithm>
#include <cstddef>
#include <memory>

template <typename T> class image {
  std::unique_ptr<T[]> m;

  std::size_t w;
  std::size_t h;

public:
  image(std::size_t width, std::size_t height)
      : m(new T[width * height]), w(width), h(height) {}

  image(const image &s) : m(new T[s.w * s.h]), w(s.w), h(s.h) {
    std::copy(&s.m[0], &s.m[0] + w * h, &m[0]);
  }

#if _MSC_VER < 1900
  image(image &&s) : m(move(s.m)), w(s.w), h(s.h) {}
#else
  image(image &&s) = default;
#endif

  image &operator=(const image &s) {
    w = s.w;
    h = s.h;
    std::unique_ptr<T[]> buffer(new T[w * h]);
    std::copy(&s.m[0], &s.m[0] + w * h, &buffer[0]);
    m = move(buffer);
    return *this;
  }

#if _MSC_VER < 1900
  image &operator=(image &&s) {
    w = s.w;
    h = s.h;
    m = move(s.m);
    return *this;
  }
#else
  image &operator=(image &&s) = default;
#endif

  T &operator()(std::size_t x, std::size_t y) const { return m[y * w + x]; }
  T &operator()(std::size_t x, std::size_t y) { return m[y * w + x]; }

  friend std::size_t width(const image &img) { return img.w; }

  friend std::size_t height(const image &img) { return img.h; }

  friend const T *data(const image &img) { return &img.m[0]; }

  friend T *data(image &img) { return &img.m[0]; }
};

#endif // INCLUDED_IMAGE
