// MIT License

// Copyright (c) 2024-2025 Cui, Xin

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

#include <ctype.h>
#include <cuda.h>
#include <cuda_runtime.h>
#include <device_launch_parameters.h>
#include <iostream>
#include <opencv2/opencv.hpp>
#include <stdio.h>
#include <string>
#include <vector>

using namespace cv;
using namespace std;

__global__ void resizeGPUNearest(const unsigned char *src, int srcWidth,
                                 int srcHeight, unsigned char *dst,
                                 int dstWidth, int dstHeight) {
  //核函数会在每个thread上运行，这里求的x、y是当前thread的坐标，同时也代表当前要处理的像素的坐标
  int y = blockIdx.y * blockDim.y + threadIdx.y;
  int x = blockIdx.x * blockDim.x + threadIdx.x;
  if (x >= dstWidth || y >= dstHeight)
    return;
  //以指针的形式操作图像，outPosition是指目标图像素在内存中的位置
  int outPosition = y * dstWidth + x;
  //求取对应原图的像素点，srcPosition是指原图像素在内存中的位置
  int srcX = x * srcWidth /
             dstWidth; //如果出现浮点数，这里就会向下取整，以此来表示最近邻
  int srcY =
      y * srcHeight / dstHeight; //（如果不喜欢向下取整，也可以选择四舍五入）
  int srcPosition = srcY * srcWidth + srcX;
  //为目标图像素赋值。RGB三通道，在内存中的位置是挨着的。
  dst[outPosition * 3 + 0] = src[srcPosition * 3 + 0];
  dst[outPosition * 3 + 1] = src[srcPosition * 3 + 1];
  dst[outPosition * 3 + 2] = src[srcPosition * 3 + 2];
}

__global__ void resizeGPUBilinear(const unsigned char *src, int srcWidth,
                                  int srcHeight, unsigned char *dst,
                                  int dstWidth, int dstHeight) {
  int y = blockIdx.y * blockDim.y + threadIdx.y;
  int x = blockIdx.x * blockDim.x + threadIdx.x;
  if (x >= dstWidth || y >= dstHeight)
    return;
  int dstOffset = y * dstWidth + x; //目标图像素在内存中的位置
  //根据缩放比例，计算在原图的坐标（浮点值）
  int srcXf = x * ((float)srcWidth / dstWidth);
  int srcYf = y * ((float)srcHeight / dstHeight);
  //向下取整，得到四个像素中，左上的像素坐标
  int srcX = (int)srcXf;
  int srcY = (int)srcYf;
  // u就是上面算法中的x-x1，1-u就是x2-x
  int u = srcXf - srcX;
  int v = srcYf - srcY;

  // P=(Q11)(x2- x)(y2-y) + (Q21)(x- x1)(y2- y) + (Q12)(x2- x)(- y1) + (Q22)(x -
  // x1)(y- y1)
  dst[dstOffset] = 0;
  dst[dstOffset] += (1 - u) * (1 - v) * src[(srcY * srcWidth + srcX)];
  dst[dstOffset] += (1 - u) * v * src[((srcY + 1) * srcWidth + srcX)];
  dst[dstOffset] += u * (1 - v) * src[(srcY * srcWidth + srcX + 1)];
  dst[dstOffset] += u * v * src[((srcY + 1) * srcWidth + srcX + 1)];
  //(srcY+1)*srcWidth+srcX+1)是右下角的像素点在内存中的位置
}

//主函数和上一个算法代码一样，唯一区别就是，为了代码简单，把图片变成了灰度图
void resizeBilinear() {
  Mat src = imread("data\\image\\house_256x256.png", 0);
  int srcWidth = src.cols;
  int srcHeight = src.rows;
  int dstWidth = 512;
  int dstHeight = 512;

  unsigned char *devSrc;
  unsigned char *devDst;

  cudaMalloc((void **)&devSrc, srcWidth * srcHeight * sizeof(unsigned char));
  cudaMalloc((void **)&devDst, dstWidth * dstHeight * sizeof(unsigned char));
  cudaMemcpy(devSrc, src.data, srcWidth * srcHeight * sizeof(unsigned char),
             cudaMemcpyHostToDevice);

  dim3 blocks((dstWidth + 15) / 16, (dstHeight + 15) / 16);
  dim3 threads(16, 16);
  resizeGPUBilinear<<<blocks, threads>>>(devSrc, srcWidth, srcHeight, devDst,
                                         dstWidth, dstHeight);

  Mat dst(Size(dstWidth, dstHeight), CV_8UC1);
  cudaMemcpy(dst.data, devDst, dstWidth * dstHeight * sizeof(unsigned char),
             cudaMemcpyDeviceToHost);

  vector<int> comprocession_params;
  comprocession_params.push_back(IMWRITE_PNG_COMPRESSION);
  comprocession_params.push_back(9);
  imwrite("resize_bilinear.png", dst, comprocession_params);
  cudaFree(devSrc);
  cudaFree(devDst);
}

void resizeNearest() {
  Mat src = imread("data\\image\\house_512x512.png"); //使用opencv
  int srcWidth = src.cols;
  int srcHeight = src.rows;
  int dstWidth = 256; //目标图的大小
  int dstHeight = 256;

  unsigned char *devSrc;
  unsigned char *devDst;

  //在GPU上为两张图申请存储空间
  cudaMalloc((void **)&devSrc,
             srcWidth * srcHeight * 3 * sizeof(unsigned char));
  cudaMalloc((void **)&devDst,
             dstWidth * dstHeight * 3 * sizeof(unsigned char));
  //把原图复制到GPU上，注意图片数据格式的变化
  cudaMemcpy(devSrc, (unsigned char *)(src.data),
             srcWidth * srcHeight * 3 * sizeof(unsigned char),
             cudaMemcpyHostToDevice);

  dim3 blocks((dstWidth + 15) / 16, (dstHeight + 15) / 16);
  dim3 threads(16, 16);
  //调用核函数，重点关注blocks与threads的设置，这样设置是为了让thread的坐标代表目标图像素的坐标
  resizeGPUNearest<<<blocks, threads>>>(devSrc, srcWidth, srcHeight, devDst,
                                        dstWidth, dstHeight);
  //将处理完的目标图拷贝回来
  Mat dst(Size(dstWidth, dstHeight), CV_8UC3);
  cudaMemcpy(dst.data, devDst, dstWidth * dstHeight * 3 * sizeof(unsigned char),
             cudaMemcpyDeviceToHost);
  //使用opencv保存新图片
  vector<int> comprocession_params;
  comprocession_params.push_back(IMWRITE_PNG_COMPRESSION);
  comprocession_params.push_back(9);
  imwrite("resize_nearest.png", dst, comprocession_params);
  cudaFree(devSrc);
  cudaFree(devDst);
}
