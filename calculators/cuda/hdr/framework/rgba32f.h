

#ifndef INCLUDED_COLOR_RGBA32F
#define INCLUDED_COLOR_RGBA32F

#pragma once

class RGBA32F {
  float color[4];

public:
  RGBA32F() = default;

  RGBA32F(float r, float g, float b, float a = 1.0f) : color{r, g, b, a} {}

  template <int i> friend float channel(const RGBA32F &color);
};

template <int i> inline float channel(const RGBA32F &color) {
  static_assert(i >= 0 && i < 4, "invalid color channel index");
  return color.color[i];
}

#endif // INCLUDED_COLOR_RGBA32F
