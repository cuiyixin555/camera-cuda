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

/**
 * @brief Scaling image kernel function for YUV420P with bilinear
 * interpolation
 * f(x,y) = f(0,0)(1-x)(1-y) + f(1,0)x(1-y) + f(0,1)(1-x)y + f(1,1)xy;
 *
 * @param pInYData input nv12 image for Y planer
 * @param pInUData input nv12 image for U planer
 * @param pInVData input nv12 image for V planer
 * @param pInWidth input image width
 * @param pInHeight input image height
 * @param pOutYData output image for Y planer
 * @param pOutUData output image for U planer
 * @param pOutVData output image for V planer
 * @param pOutWidth output image width
 * @param pOutHeight output image height
 */
__global__ void ReSizeKernel_Bilinear_YUV420P(
    unsigned char *pInYData, unsigned char *pInUData, unsigned char *pInVData,
    int pInWidth, int pInHeight, unsigned char *pOutYData,
    unsigned char *pOutUData, unsigned char *pOutVData, int pOutWidth,
    int pOutHeight) {
  int tidx = threadIdx.x + blockDim.x * blockIdx.x;
  int tidy = threadIdx.y + blockDim.y * blockIdx.y;

  if (tidx < pOutWidth && tidy < pOutHeight) {
    float srcX = tidx * ((float)(pInWidth - 1) / (pOutWidth - 1));
    float srcY = tidy * ((float)(pInHeight - 1) / (pOutHeight - 1));

    // 计算取图像坐标
    int fx0 = srcX;
    int fy0 = srcY;
    int fx1 = srcX > fx0 ? fx0 + 1 : fx0;
    int fy1 = srcY > fy0 ? fy0 + 1 : fy0;

    // 计算取像素比例
    float xProportion = srcX - fx0;
    float yProportion = srcY - fy0;

    // 四个输入坐标
    int idx_in_y_00 = fy0 * pInWidth + fx0;
    int idx_in_uv_00 = fy0 / 2 * pInWidth / 2 + fx0 / 2;

    int idx_in_y_10 = fy1 * pInWidth + fx0;
    int idx_in_uv_10 = fy1 / 2 * pInWidth / 2 + fx0 / 2;

    int idx_in_y_01 = fy0 * pInWidth + fx1;
    int idx_in_uv_01 = fy0 / 2 * pInWidth / 2 + fx1 / 2;

    int idx_in_y_11 = fy1 * pInWidth + fx1;
    int idx_in_uv_11 = fy1 / 2 * pInWidth / 2 + fx1 / 2;

    // 输出坐标
    int idx_out_y = tidy * pOutWidth + tidx;
    int idx_out_uv = tidy / 2 * pOutWidth / 2 + tidx / 2;

    // Y
    pOutYData[idx_out_y] =
        pInYData[idx_in_y_00] * (1 - xProportion) * (1 - yProportion) +
        pInYData[idx_in_y_10] * xProportion * (1 - yProportion) +
        pInYData[idx_in_y_01] * (1 - xProportion) * yProportion +
        pInYData[idx_in_y_11] * xProportion * yProportion;

    // U
    pOutUData[idx_out_uv] =
        pInUData[idx_in_uv_00] * (1 - xProportion) * (1 - yProportion) +
        pInUData[idx_in_uv_10] * xProportion * (1 - yProportion) +
        pInUData[idx_in_uv_01] * (1 - xProportion) * yProportion +
        pInUData[idx_in_uv_11] * xProportion * yProportion;

    // V
    pOutVData[idx_out_uv] =
        pInVData[idx_in_uv_00] * (1 - xProportion) * (1 - yProportion) +
        pInVData[idx_in_uv_10] * xProportion * (1 - yProportion) +
        pInVData[idx_in_uv_01] * (1 - xProportion) * yProportion +
        pInVData[idx_in_uv_11] * xProportion * yProportion;
  }
}

/**
 * @brief Modify size with bilinear interpolation YUV420P
 *
 * @param frame input image
 * @param width input width
 * @param height input height
 * @return AVFrame*
 */
AVFrame *ReSize_Bilinear_YUV420P(AVFrame *frame, int width, int height) {
  auto img_size_y = width * height * sizeof(unsigned char);
  auto img_size_uv = (width / 2) * (height / 2) * sizeof(unsigned char);

  AVFrame *dstImg;
  unsigned char *outputY = nullptr;
  unsigned char *outputU = nullptr;
  unsigned char *outputV = nullptr;

  dstImg = av_frame_alloc();
  av_image_alloc(dstImg->data, dstImg->linesize, width, height,
                 (AVPixelFormat)frame->format, 1);
  dstImg->width = width;
  dstImg->height = height;
  dstImg->format = (AVPixelFormat)frame->format;

  cudaMalloc(&outputY, img_size_y);
  cudaMalloc(&outputU, img_size_uv);
  cudaMalloc(&outputV, img_size_uv);

  dim3 block(32, 32);
  dim3 grid((width + block.x - 1) / block.x, (height + block.y - 1) / block.y);
  ReSizeKernel_Bilinear_YUV420P<<<grid, block>>>(
      frame->data[0], frame->data[1], frame->data[2], frame->width,
      frame->height, outputY, outputU, outputV, width, height);
  cudaThreadSynchronize();

  // 图像从 Gpu 拷贝到 Cpu
  cudaMemcpy(dstImg->data[0], outputY, img_size_y, cudaMemcpyDeviceToHost);
  cudaMemcpy(dstImg->data[1], outputU, img_size_uv, cudaMemcpyDeviceToHost);
  cudaMemcpy(dstImg->data[2], outputV, img_size_uv, cudaMemcpyDeviceToHost);
  return dstImg;
}

/**
 * @brief Scaling image kernel function NV12
 * Bilinear interpolation
 * f(x,y) = f(0,0)(1-x)(1-y) + f(1,0)x(1-y) + f(0,1)(1-x)y + f(1,1)xy;
 *
 * @param pInYData input image for Y planer
 * @param pInUVData input image for UV planer
 * @param pInWidth  input image width
 * @param pInHeight input image height
 * @param pOutYData output image for Y planer
 * @param pOutUVData output image for UV planer
 * @param pOutWidth output image width
 * @param pOutHeight output image height
 */
__global__ void ReSizeKernel_Bilinear_NV12(unsigned char *pInYData,
                                           unsigned char *pInUVData,
                                           int pInWidth, int pInHeight,
                                           unsigned char *pOutYData,
                                           unsigned char *pOutUVData,
                                           int pOutWidth, int pOutHeight) {
  int tidx = threadIdx.x + blockDim.x * blockIdx.x;
  int tidy = threadIdx.y + blockDim.y * blockIdx.y;

  if (tidx < pOutWidth && tidy < pOutHeight) {
    float srcX = tidx * ((float)(pInWidth - 1) / (pOutWidth - 1));
    float srcY = tidy * ((float)(pInHeight - 1) / (pOutHeight - 1));

    /// calculate image coordinates
    int fx0 = srcX;
    int fy0 = srcY;
    int fx1 = srcX > fx0 ? fx0 + 1 : fx0;
    int fy1 = srcY > fy0 ? fy0 + 1 : fy0;

    /// calculate pixel ratio
    float xProportion = srcX - fx0;
    float yProportion = srcY - fy0;

    /// four input coordinates
    int idx_in_y_00 = fy0 * pInWidth + fx0;
    int idx_in_uv_00 = fy0 / 2 * pInWidth + fx0;

    int idx_in_y_10 = fy1 * pInWidth + fx0;
    int idx_in_uv_10 = fy1 / 2 * pInWidth + fx0;

    int idx_in_y_01 = fy0 * pInWidth + fx1;
    int idx_in_uv_01 = fy0 / 2 * pInWidth + fx1;

    int idx_in_y_11 = fy1 * pInWidth + fx1;
    int idx_in_uv_11 = fy1 / 2 * pInWidth + fx1;

    /// output coordinates
    int idx_out_y = tidy * pOutWidth + tidx;
    int idx_out_uv = tidy / 2 * pOutWidth + tidx;

    // Y
    pOutYData[idx_out_y] =
        pInYData[idx_in_y_00] * (1 - xProportion) * (1 - yProportion) +
        pInYData[idx_in_y_10] * xProportion * (1 - yProportion) +
        pInYData[idx_in_y_01] * (1 - xProportion) * yProportion +
        pInYData[idx_in_y_11] * xProportion * yProportion;

    // U
    pOutUVData[tidx % 2 == 0 ? idx_out_uv : idx_out_uv - 1] =
        pInUVData[fx0 % 2 == 0 ? idx_in_uv_00 : idx_in_uv_00 - 1] *
            (1 - xProportion) * (1 - yProportion) +
        pInUVData[fx0 % 2 == 0 ? idx_in_uv_10 : idx_in_uv_10 - 1] *
            xProportion * (1 - yProportion) +
        pInUVData[fx1 % 2 == 0 ? idx_in_uv_01 : idx_in_uv_01 - 1] *
            (1 - xProportion) * yProportion +
        pInUVData[fx1 % 2 == 0 ? idx_in_uv_11 : idx_in_uv_11 - 1] *
            xProportion * yProportion;

    // V
    pOutUVData[tidx % 2 == 0 ? idx_out_uv + 1 : idx_out_uv] =
        pInUVData[fx0 % 2 == 0 ? idx_in_uv_00 + 1 : idx_in_uv_00] *
            (1 - xProportion) * (1 - yProportion) +
        pInUVData[fx0 % 2 == 0 ? idx_in_uv_10 + 1 : idx_in_uv_10] *
            xProportion * (1 - yProportion) +
        pInUVData[fx1 % 2 == 0 ? idx_in_uv_01 + 1 : idx_in_uv_01] *
            (1 - xProportion) * yProportion +
        pInUVData[fx1 % 2 == 0 ? idx_in_uv_11 + 1 : idx_in_uv_11] *
            xProportion * yProportion;
  }
}

/**
 * @brief Modify size bilinear interpolation NV12
 *
 * @param frame input image
 * @param width input image width
 * @param height input image height
 * @return AVFrame*
 */
AVFrame *ReSize_Bilinear_NV12(AVFrame *frame, int width, int height) {
  auto img_size_y = width * height * sizeof(unsigned char);
  auto img_size_uv = width * (height / 2) * sizeof(unsigned char);

  AVFrame *dstImg;
  unsigned char *outputY = nullptr;
  unsigned char *outputUV = nullptr;

  dstImg = av_frame_alloc();
  av_image_alloc(dstImg->data, dstImg->linesize, width, height,
                 (AVPixelFormat)frame->format, 1);
  dstImg->width = width;
  dstImg->height = height;
  dstImg->format = (AVPixelFormat)frame->format;

  cudaMalloc(&outputY, img_size_y);
  cudaMalloc(&outputUV, img_size_uv);

  dim3 block(32, 32);
  dim3 grid((width + block.x - 1) / block.x, (height + block.y - 1) / block.y);
  ReSizeKernel_Bilinear_NV12<<<grid, block>>>(frame->data[0], frame->data[1],
                                              frame->width, frame->height,
                                              outputY, outputUV, width, height);
  cudaThreadSynchronize();

  /// Image copy from GPU to CPU
  cudaMemcpy(dstImg->data[0], outputY, img_size_y, cudaMemcpyDeviceToHost);
  cudaMemcpy(dstImg->data[1], outputUV, img_size_uv, cudaMemcpyDeviceToHost);
  return dstImg;
}