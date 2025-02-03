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

#include <algorithm>
#include <cmath>
#include <cstdint>
#include <exception>
#include <iomanip>
#include <iostream>


#include <cuda_runtime_api.h>

#include <framework/CUDA/error.h>
#include <framework/cmd_args.h>
#include <framework/image.h>
#include <framework/pfm.h>
#include <framework/png.h>
#include <framework/rgb32f.h>


#include "HDRPipeline.h"

namespace {
std::uint8_t toLinear8(float c) {
  return static_cast<std::uint8_t>(std::min(std::max(c, 0.0f), 1.0f) * 255.0f);
}

std::uint8_t toSRGB8(float c) { return toLinear8(std::pow(c, 1.0f / 2.2f)); }

image<std::uint32_t> tonemap(const image<float> &img) {
  image<std::uint32_t> output(width(img), height(img));

  for (std::size_t y = 0U; y < height(img); ++y) {
    for (std::size_t x = 0U; x < width(img); ++x) {
      std::uint8_t v = toSRGB8(img(x, y));
      output(x, y) = 0xFF000000U | (v << 16) | (v << 8) | v;
    }
  }
  return output;
}

image<std::uint32_t> tonemap(const image<RGB32F> &img) {
  image<std::uint32_t> output(width(img), height(img));

  for (std::size_t y = 0U; y < height(img); ++y)
    for (std::size_t x = 0U; x < width(img); ++x)
      output(x, y) = 0xFF000000U | (toSRGB8(channel<2>(img(x, y))) << 16) |
                     (toSRGB8(channel<1>(img(x, y))) << 8) |
                     toSRGB8(channel<0>(img(x, y)));
  return output;
}
} // namespace

int main(int argc, char *argv[]) {
  try {
    const char *input_file = nullptr;
    int cuda_device = 0;
    float exposure_value = 0.0f;
    float brightpass_threshold = 0.9f;
    int test_runs = 1;

    for (char **a = &argv[1]; *a; ++a) {
      if (!checkArgument("--device", a, cuda_device))
        if (!checkArgument("--exposure", a, exposure_value))
          if (!checkArgument("--brightpass", a, brightpass_threshold))
            if (!checkArgument("--test-runs", a, test_runs))
              input_file = *a;
    }

    if (!input_file)
      throw usage_error("need to specify input file");

    auto input = PFM::loadRGB32F(input_file);
    float exposure = std::exp2(exposure_value);

    cudaDeviceProp props;
    throw_error(cudaGetDeviceProperties(&props, cuda_device));
    std::cout << "using cuda device " << cuda_device
              << ":\n"
                 "\t"
              << props.name
              << "\n"
                 "\tcompute capability "
              << props.major << "." << props.minor << " @ "
              << std::setprecision(1) << std::fixed << props.clockRate / 1000.0f
              << " MHz\n"
                 "\t"
              << props.multiProcessorCount
              << " multiprocessors\n"
                 "\t"
              << props.totalGlobalMem / (1024U * 1024U)
              << " MiB global memory  "
              << props.sharedMemPerMultiprocessor / 1024
              << " kiB shared memory\n"
              << std::endl;

    throw_error(cudaSetDevice(cuda_device));

    cudaEvent_t pipeline_consume, luminance_begin, luminance_end,
        downsample_begin, downsample_end, tonemap_begin, tonemap_end,
        blur_begin, blur_end, compose_begin, compose_end;
    throw_error(cudaEventCreate(&pipeline_consume));
    throw_error(cudaEventCreate(&luminance_begin));
    throw_error(cudaEventCreate(&luminance_end));
    throw_error(cudaEventCreate(&downsample_begin));
    throw_error(cudaEventCreate(&downsample_end));
    throw_error(cudaEventCreate(&tonemap_begin));
    throw_error(cudaEventCreate(&tonemap_end));
    throw_error(cudaEventCreate(&blur_begin));
    throw_error(cudaEventCreate(&blur_end));
    throw_error(cudaEventCreate(&compose_begin));
    throw_error(cudaEventCreate(&compose_end));

    HDRPipeline pipeline(static_cast<unsigned int>(width(input)),
                         static_cast<unsigned int>(height(input)));

    float luminance_time = 0.0f;
    float downsample_time = 0.0f;
    float tonemap_time = 0.0f;
    float blur_time = 0.0f;
    float compose_time = 0.0f;
    float overall_time = 0.0f;

    for (int i = 0; i < test_runs; ++i) {
      throw_error(cudaEventRecord(pipeline_consume));
      pipeline.consume(reinterpret_cast<const float *>(data(input)));

      throw_error(cudaEventRecord(luminance_begin));
      pipeline.computeLuminance();
      throw_error(cudaEventRecord(luminance_end));

      throw_error(cudaEventRecord(downsample_begin));
      float lum = pipeline.downsample();
      throw_error(cudaEventRecord(downsample_end));

      throw_error(cudaEventRecord(tonemap_begin));
      pipeline.tonemap(exposure / lum, brightpass_threshold);
      throw_error(cudaEventRecord(tonemap_end));

      throw_error(cudaEventRecord(blur_begin));
      pipeline.blur();
      throw_error(cudaEventRecord(blur_end));

      throw_error(cudaEventRecord(compose_begin));
      pipeline.compose();
      throw_error(cudaEventRecord(compose_end));

      throw_error(cudaEventSynchronize(compose_end));

      float t;
      throw_error(cudaEventElapsedTime(&t, luminance_begin, luminance_end));
      luminance_time += t;
      throw_error(cudaEventElapsedTime(&t, downsample_begin, downsample_end));
      downsample_time += t;
      throw_error(cudaEventElapsedTime(&t, tonemap_begin, tonemap_end));
      tonemap_time += t;
      throw_error(cudaEventElapsedTime(&t, blur_begin, blur_end));
      blur_time += t;
      throw_error(cudaEventElapsedTime(&t, compose_begin, compose_end));
      compose_time += t;
      throw_error(cudaEventElapsedTime(&t, pipeline_consume, compose_end));
      overall_time += t;
    }

    {
      float t_norm = 1.0f / test_runs;
      luminance_time *= t_norm;
      downsample_time *= t_norm;
      tonemap_time *= t_norm;
      blur_time *= t_norm;
      compose_time *= t_norm;
      overall_time *= t_norm;
    }

    std::cout << "-------------------------------------------------------------"
                 "-----------\n"
              << std::setprecision(2) << std::fixed
              << "luminance:      " << luminance_time
              << " ms\n"
                 "downsampling:   "
              << downsample_time
              << " ms\n"
                 "tonemapping:    "
              << tonemap_time
              << " ms\n"
                 "blur:           "
              << blur_time
              << " ms\n"
                 "compositing:    "
              << compose_time
              << " ms\n"
                 "overall:        "
              << overall_time << " ms\n";

    auto luminance = pipeline.readLuminance();
    PFM::saveR32F("luminance.pfm", luminance);
    PNG::saveImage("luminance.png", tonemap(luminance));

    auto downsample = pipeline.readDownsample();
    PFM::saveR32F("downsample.pfm", downsample);
    PNG::saveImage("downsample.png", tonemap(downsample));

    auto tonemapped = pipeline.readTonemapped();
    PFM::saveRGB32F("tonemapped.pfm", tonemapped);
    PNG::saveImage("tonemapped.png", tonemap(tonemapped));

    auto brightpass = pipeline.readBrightpass();
    PFM::saveRGB32F("brightpass.pfm", brightpass);
    PNG::saveImage("brightpass.png", tonemap(brightpass));

    auto blurred = pipeline.readBlurred();
    PFM::saveRGB32F("blurred.pfm", blurred);
    PNG::saveImage("blurred.png", tonemap(blurred));

    auto output = pipeline.readOutput();
    PFM::saveRGB32F("output.pfm", output);
    PNG::saveImage("output.png", tonemap(output));
  } catch (const usage_error &e) {
    std::cout << "error: " << e.what() << std::endl;
    std::cout << "usage: hdr_pipeline {options} <input-file>\n"
                 "\toptions:\n"
                 "\t  --device <i>           use CUDA device <i>, default: 0\n"
                 "\t  --exposure <v>         set exposure value to <v>, "
                 "default: 0.0\n"
                 "\t  --brightpass <v>       set brightpass threshold to <v>, "
                 "default: 0.9\n"
                 "\t  --test-runs <N>        average timings over <N> test "
                 "runs, default: 1\n";
    return -127;
  } catch (std::exception &e) {
    std::cout << "error: " << e.what() << std::endl;
    return -1;
  } catch (...) {
    std::cout << "unknown exception" << std::endl;
    return -128;
  }

  return 0;
}
