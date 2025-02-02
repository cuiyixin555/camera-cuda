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

/// <summary>
/// 修改图像大小
/// </summary>
/// <param name="frame">输入图像</param>
/// <param name="width">修改宽度</param>
/// <param name="height">修改高度</param>
/// <param name="type">0:最近邻插值 1:双线性差值</param>
/// <returns>修改后图像</returns>

/**
 * @brief
 *
 */
extern "C" AVFrame *ReSize(AVFrame *frame, int width, int height, int type) {
  AVFrame *outFrame;

  switch (frame->format) {
  case AV_PIX_FMT_YUV420P:
    if (type == 0) {
      outFrame = ReSize_Nearest_YUV420P(frame, width, height);
    } else if (type == 1) {
      outFrame = ReSize_Bilinear_YUV420P(frame, width, height);
    }
    break;
  case AV_PIX_FMT_NV12:
    if (type == 0) {
      outFrame = ReSize_Nearest_NV12(frame, width, height);
    } else if (type == 1) {
      outFrame = ReSize_Bilinear_NV12(frame, width, height);
    }
    break;
  default:
    break;
  }

  return outFrame;
}
