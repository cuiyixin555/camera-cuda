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

#include "calculators/cuda/hdr/color.cuh"

namespace {
constexpr unsigned int divup(unsigned int a, unsigned int b) {
  return (a + b - 1) / b;
}

template <typename T> void swap(T &a, T &b) {
  T c = b;
  b = a;
  a = c;
}
} // namespace

// kernel that computes average luminance of a pixel
__global__ void luminance_kernel(float *dest, const float *input,
                                 unsigned int width, unsigned int height) {
  // each thread needs to know on which pixels to work -> get absolute
  // coordinates of the thread in the grid
  unsigned int x = blockIdx.x * blockDim.x + threadIdx.x;
  unsigned int y = blockIdx.y * blockDim.y + threadIdx.y;

  // input is stored as array (row-wise). Load the pixel values
  // offset of first pixel  = y coordinates * width of block (b/c full rows
  // already read) + x coordinates

  if (x < width && y < height) {
    const float *input_pixel =
        input + 3 * (width * y + x); // *3 b/c three colors (=bytes) per pixel.

    float lum = 0.21f * input_pixel[0] + 0.72f * input_pixel[1] +
                0.07f * input_pixel[2]; // compensate for human vision

    dest[width * y + x] =
        lum; // store the results. not * 3 b/c only luminance, no colors
  }
}

// get the required number of blocks to cover the whole image, and run the
// kernel on all blocks
void luminance(float *dest, const float *input, unsigned int width,
               unsigned int height) {
  const dim3 block_size = {32, 32};
  // calculate number of blocks required to process the whole image -> round up
  // to the next multiple of 32 (full block)
  const dim3 num_blocks = {divup(width, block_size.x),
                           divup(height, block_size.y)};

  // launch the kernel that we wrote above for all the blocks
  luminance_kernel<<<num_blocks, block_size>>>(dest, input, width, height);

  // this 'downsampling' step takes about 1.3ms on a GT 730m
  // now we want to run this in a hierarchical way: reduce the image in each
  // step
  // -> launch blocks on the images, reducing its size each step until we end up
  // with a 1x1 color
  // -> we need to allocate memory for a buffer in the HDRPipeline object
  // declaration
}

__global__ void downsample_kernel(float *dest, float *input, unsigned int width,
                                  unsigned int height, unsigned int outputPitch,
                                  unsigned int inputPitch) {
  // each thread needs to know on which pixels to work -> get absolute
  // coordinates for/of the thread in the grid
  unsigned int x = blockIdx.x * blockDim.x + threadIdx.x;
  unsigned int y = blockIdx.y * blockDim.y + threadIdx.y;

  int F = 2; // width of downsampling square (F^2 = number of pixels to be
             // pooled together)

  // printf(" KERNEL width %d | height %d \n", width, height);
  if ((x * F > width - 1) || (y * F > height - 1))
    return;

  float sum = 0.0f;

  int nb_counted = F * F;
  // number of pixels to be counted: We start at pixel (x*F, y*F), continue till
  // coordinate(width-1, height-1)
  int xDim = min(F, width - x * F);
  int yDim = min(F, height - y * F);
  // printf("xDim: %d, yDim: %d \n", xDim, yDim);
  nb_counted = xDim * yDim;
  // printf(" counted %d | width %d | height %d | \t x %d | y %d \n",
  // nb_counted, width, height, x*F, y*F);

  // 2D version: add pixels in 2x2 block, calculate the average. Jump with F so
  // different threads don't operate on the same pixels.
  for (int j = 0; j < F; j++) {
    for (int i = 0; i < F; i++) {
      // current pixel : (x*F+i, y*F+j)
      if ((y * F + j) < height &&
          (x * F + i) <
              width) { // only sum pixels inside the image, don't count
                       // overlapping at the right or bottom sides
        sum += input[(y * F + j) * inputPitch + x * F + i];
      }
    }
  }
  dest[y * outputPitch + x] = sum / nb_counted;
}

// get the required number of blocks to cover the whole image, and run the
// kernel on all blocks
float downsample(float *dest, float *luminance, unsigned int width,
                 unsigned int height) {
  const dim3 block_size = {32, 32};
  // calculate number of blocks required to process the whole image -> round up
  // to the next multiple of 32 (full block)
  const dim3 num_blocks = {divup(width, block_size.x),
                           divup(height, block_size.y)};

  // Store original width = width of buffer
  const unsigned int pitchBuf = width / 2;
  const unsigned int pitchLuminance = width;

  // first iteration
  downsample_kernel<<<num_blocks, block_size>>>(dest, luminance, width, height,
                                                pitchBuf, pitchLuminance);
  int ping = 0; // result in dest buffer

  while (width != 1 || height != 1) {
    // result will be in the dest buffer
    width = width / 2;
    height = height / 2;
    if (width < 1) {
      width = 1;
      //			printf("width < 1 \n");
    }
    if (height < 1) {
      height = 1;
      //			printf("height < 1 \n");
    }

    //		printf(" width %d | height %d \n", width, height);
    if (ping) {
      downsample_kernel<<<num_blocks, block_size>>>(
          dest, luminance, width, height, pitchBuf, pitchLuminance);
    } else {
      // now ping-pong; result will be in the luminance buffer
      downsample_kernel<<<num_blocks, block_size>>>(
          luminance, dest, width, height, pitchLuminance, pitchBuf);
    }
    ping = !ping;
  }

  // make sure the result is stored in dest
  if (ping) {
    cudaMemcpy(luminance, dest, 1, cudaMemcpyDeviceToDevice);
  }

  // return the grayscale value
  float average;
  cudaMemcpy(&average, dest, sizeof(float), cudaMemcpyDeviceToHost);
  return average;
}

// first do it on the x direction
// then do it on the y direction (-> texture memory to make this fast)
__constant__ float weights[] = { // weights in one dimension -> 33x33 filter
    //  -16           -15         -14          -13          -12          -11 -10
    //  -9           -8           -7           -6           -5           -4 -3
    //  -2           -1
    0.00288204f, 0.00418319f, 0.00592754f, 0.00819980f, 0.01107369f,
    0.01459965f, 0.01879116f, 0.02361161f, 0.02896398f, 0.03468581f,
    0.04055144f, 0.04628301f, 0.05157007f, 0.05609637f, 0.05957069f,
    0.06175773f,
    //    0
    0.06250444f,
    //    1             2           3            4            5            6 7
    //    8            9           10           11           12           13 14
    //    15           16
    0.06175773f, 0.05957069f, 0.05609637f, 0.05157007f, 0.04628301f,
    0.04055144f, 0.03468581f, 0.02896398f, 0.02361161f, 0.01879116f,
    0.01459965f, 0.01107369f, 0.00819980f, 0.00592754f, 0.00418319f,
    0.00288204f};

__global__ void blur_kernel_x(float *dest, const float *src, unsigned int width,
                              unsigned int height, unsigned int inputPitch,
                              unsigned int outputPitch) {
  // 1 thread per output pixel
  unsigned int x = blockIdx.x * blockDim.x + threadIdx.x;
  unsigned int y = blockIdx.y * blockDim.y + threadIdx.y;

  //	__shared__ float blur_buffer[65*65];
  //	if (threadIdx.x == 0){
  //		// read 64x64 input image into shared buffer
  //		// make sure you don't read out of bounds when the kernels
  //operate near the image borders 		int startx = max(0,x - 64);
  // int endx = min(width-1, x+64);
  //
  //		int starty = max(0,y - 64);
  //		int endy = min(height-1, y+64);
  //
  //		blur_buffer = src[startx:endx][starty:endy];
  //		for (int i = 0; i < 65; i++){
  //			for (int j = 0; j< 65){
  //				// if the kernel is near the borders, you need to fill
  //the buffer with zeros at the right places
  //				// so the access of the  buffer is easy relative to
  //the thread coordinates.
  //			}
  //		}
  //	}

  float sumR = 0.0f;
  float sumG = 0.0f;
  float sumB = 0.0f;

  for (int i = -16; i <= 16; i++) {
    if ((3 * (x + i) > 0) && (3 * (x + i) < 3 * width)) {
      sumR += src[3 * y * inputPitch + 3 * (x + i)] * weights[i + 16];
      sumG += src[3 * y * inputPitch + 3 * (x + i) + 1] * weights[i + 16];
      sumB += src[3 * y * inputPitch + 3 * (x + i) + 2] * weights[i + 16];
    }
  }
  //	if (!threadIdx.x){
  //		printf("sumR: %f | sumG: %f | sumB: %f \n", sumR, sumG, sumB);
  //	}

  dest[3 * y * outputPitch + 3 * x] = sumR;
  dest[3 * y * outputPitch + 3 * x + 1] = sumG;
  dest[3 * y * outputPitch + 3 * x + 2] = sumB;
}

__global__ void blur_kernel_y(float *dest, const float *src, unsigned int width,
                              unsigned int height, unsigned int inputPitch,
                              unsigned int outputPitch) {

  // 1 thread per output pixel
  unsigned int x = blockIdx.x * blockDim.x + threadIdx.x;
  unsigned int y = blockIdx.y * blockDim.y + threadIdx.y;

  float sumR = 0.0f;
  float sumG = 0.0f;
  float sumB = 0.0f;

  for (int i = -16; i <= 16; i++) {
    if ((3 * (y + i) > 0) && (3 * (y + i) < 3 * height)) {
      // all threads in the block read the same pixels. -> lots of overlap.
      // each block accesses 32x32 pixels plus 16 on each side for the gaussian
      // operation -> 64x64 pixels are to be read Before processing, have the
      // threads each read 4 pixels (32x32 threads, 64x64 pixels) in a shared
      // buffer. Then Sync, and have the threads process data from this shared
      // buffer instead of from memory.

      sumR += src[3 * (y + i) * inputPitch + 3 * x] * weights[i + 16];
      sumG += src[3 * (y + i) * inputPitch + 3 * x + 1] * weights[i + 16];
      sumB += src[3 * (y + i) * inputPitch + 3 * x + 2] * weights[i + 16];
    }
  }

  dest[3 * y * outputPitch + 3 * x] = sumR;
  dest[3 * y * outputPitch + 3 * x + 1] = sumG;
  dest[3 * y * outputPitch + 3 * x + 2] = sumB;
}

void gaussian_blur(float *dest, const float *src, unsigned int width,
                   unsigned int height) {
  const dim3 block_size = {32, 32};
  // calculate number of blocks required to process the whole image -> round up
  // to the next multiple of 32 (full block)
  const dim3 num_blocks = {divup(width, block_size.x),
                           divup(height, block_size.y)};

  int inputPitch = width;
  int outputPitch = width;

  blur_kernel_x<<<num_blocks, block_size>>>(dest, src, width, height,
                                            inputPitch, outputPitch);
  blur_kernel_y<<<num_blocks, block_size>>>(dest, dest, width, height,
                                            inputPitch, outputPitch);
}

void compose(float *output, const float *tonemapped, const float *blurred,
             unsigned int width, unsigned int height) {
  // TODO: add blurred brightpass to tonemapped image
}

__global__ void tonemap_kernel(float *tonemapped, float *brightpass,
                               const float *src, unsigned int width,
                               unsigned int height, float exposure,
                               float brightpass_threshold) {
  unsigned int x = blockIdx.x * blockDim.x + threadIdx.x;
  unsigned int y = blockIdx.y * blockDim.y + threadIdx.y;

  if (x < width && y < height) {
    // figure out input color
    math::float3 c = {src[3 * (y * width + x) + 0],
                      src[3 * (y * width + x) + 1],
                      src[3 * (y * width + x) + 2]};

    // compute tonemapped color
    math::float3 c_t = tonemap(c, exposure);

    // write out tonemapped color
    tonemapped[3 * (y * width + x) + 0] = c_t.x;
    tonemapped[3 * (y * width + x) + 1] = c_t.y;
    tonemapped[3 * (y * width + x) + 2] = c_t.z;

    // write out brightpass color
    math::float3 c_b = luminance(c_t) > brightpass_threshold
                           ? c_t
                           : math::float3{0.0f, 0.0f, 0.0f};
    brightpass[3 * (y * width + x) + 0] = c_b.x;
    brightpass[3 * (y * width + x) + 1] = c_b.y;
    brightpass[3 * (y * width + x) + 2] = c_b.z;
  }
}

void tonemap(float *tonemapped, float *brightpass, const float *src,
             unsigned int width, unsigned int height, float exposure,
             float brightpass_threshold) {
  const auto block_size = dim3{32U, 32U};

  auto num_blocks =
      dim3{divup(width, block_size.x), divup(height, block_size.y)};

  tonemap_kernel<<<num_blocks, block_size>>>(tonemapped, brightpass, src, width,
                                             height, exposure,
                                             brightpass_threshold);
}
