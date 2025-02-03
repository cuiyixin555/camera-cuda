

#ifndef INCLUDED_FRAMEWORK_PNG_FILE_FORMAT
#define INCLUDED_FRAMEWORK_PNG_FILE_FORMAT

#pragma once

#include <cstdint>
#include <fstream>
#include <tuple>


#include <png.h>

#include "image.h"

namespace PNG {
class IStream {
  std::ifstream file;
  png_struct *png_ptr;
  png_info *info_ptr;

public:
  IStream(const IStream &) = delete;
  IStream &operator=(const IStream &) = delete;

  IStream(const char *filename);
  ~IStream();

  operator const png_struct *() const { return png_ptr; }
  operator png_struct *() { return png_ptr; }

  operator const png_info *() const { return info_ptr; }
  operator png_info *() { return info_ptr; }
};

class OStream {
  std::ofstream file;
  png_struct *png_ptr;
  png_info *info_ptr;

public:
  OStream(const OStream &) = delete;
  OStream &operator=(const OStream &) = delete;

  OStream(const char *filename);
  ~OStream();

  operator const png_struct *() const { return png_ptr; }
  operator png_struct *() { return png_ptr; }

  operator const png_info *() const { return info_ptr; }
  operator png_info *() { return info_ptr; }
};

class RGBA8OStream : public OStream {
public:
  RGBA8OStream(const char *filename, size_t width, size_t height);
  ~RGBA8OStream();

  void writeRow(const std::uint32_t *row);
};

std::tuple<int, int> readImageSize(const char *filename);

image<std::uint32_t> loadImage2D(const char *filename);
void saveImage(const char *filename, const image<std::uint32_t> &surface);
} // namespace PNG

#endif // INCLUDED_FRAMEWORK_PNG_FILE_FORMAT
