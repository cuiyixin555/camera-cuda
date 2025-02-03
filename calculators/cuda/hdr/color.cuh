// MIT License

// Copyright (c) 2024 Cui, Xin

// Permission is hereby granted, free of charge, to any person obtaining a copy
// of this software and associated documentation files (the "Software"), to deal
// in the Software without restriction, including without limitation the rights
// to use, copy, modify, merge, publish, distribute, sublicense, and/or sell
// copies of the Software, and to permit persons to whom the Software is
// furnished to do so, subject to the following conditions:

// The above copyright notice and this permission notice shall be included in
// all copies or substantial portions of the Software.

// THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
// IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
// FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
// AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
// LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
// OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE
// SOFTWARE.
//

#include <math/vector.h>

__device__ float luminance(const math::float3 &color) {
  return dot(color, math::float3{0.2126f, 0.7152f, 0.0722f});
}

__device__ float Uncharted2Tonemap(float x) {
  // from http://filmicworlds.com/blog/filmic-tonemapping-operators/
  constexpr float A = 0.15;
  constexpr float B = 0.50;
  constexpr float C = 0.10;
  constexpr float D = 0.20;
  constexpr float E = 0.02;
  constexpr float F = 0.30;
  return ((x * (A * x + C * B) + D * E) / (x * (A * x + B) + D * F)) - E / F;
}

__device__ float tonemap(float c, float exposure) {
  constexpr float W = 11.2;
  return Uncharted2Tonemap(c * exposure * 2.0f) / Uncharted2Tonemap(W);
}

__device__ math::float3 tonemap(const math::float3 &c, float exposure) {
  return {tonemap(c.x, exposure), tonemap(c.y, exposure),
          tonemap(c.z, exposure)};
}

__device__ unsigned char toLinear8(float c) {
  return static_cast<unsigned char>(saturate(c) * 255.0f);
}

__device__ unsigned char toSRGB8(float c) {
  return toLinear8(powf(c, 1.0f / 2.2f));
}

__device__ float fromLinear8(unsigned char c) { return c * (1.0f / 255.0f); }

__device__ float fromSRGB8(unsigned char c) {
  return powf(fromLinear8(c), 2.2f);
}
