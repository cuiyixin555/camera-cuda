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

#ifndef INCLUDED_HDRPIPELINE
#define INCLUDED_HDRPIPELINE

#pragma once

#include <memory>

#include <cuda_runtime_api.h>

#include "calculators/cuda/hdr/framework/image.h"
#include "calculators/cuda/hdr/framework/rgb32f.h"

struct cudaFreeDeleter {
  void operator()(void *ptr) const { cudaFree(ptr); }
};

template <typename T>
using cuda_unique_ptr = std::unique_ptr<T, cudaFreeDeleter>;

class HDRPipeline {
  const unsigned int width;
  const unsigned int height;

  cuda_unique_ptr<float> d_input_image;
  cuda_unique_ptr<float> d_luminance_image;
  cuda_unique_ptr<float> d_downsample_buffer;
  cuda_unique_ptr<float> d_tonemapped_image;
  cuda_unique_ptr<float> d_brightpass_image;
  cuda_unique_ptr<float> d_blurred_image;
  cuda_unique_ptr<float> d_output_image;

public:
  HDRPipeline(unsigned int width, unsigned int height);

  void consume(const float *input_image);
  void computeLuminance();
  float downsample();
  void tonemap(float exposure, float brightpass_threshold);
  void blur();
  void compose();

  image<float> readLuminance();
  image<float> readDownsample();
  image<RGB32F> readTonemapped();
  image<RGB32F> readBrightpass();
  image<RGB32F> readBlurred();
  image<RGB32F> readOutput();
};

#endif // INCLUDED_HDRPIPELINE
