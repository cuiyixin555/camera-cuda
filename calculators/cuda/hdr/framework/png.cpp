

#include <memory>
#include <stdexcept>
#include <string>


#include "calculators/cuda/hdr/framework/png.h"

namespace {
void error_fn(png_structp png_ptr, png_const_charp error_msg) {
  throw std::runtime_error(error_msg);
}

void warning_fn(png_structp png_ptr, png_const_charp warning_msg) {}

void read_fn(png_structp png_ptr, png_bytep data, png_size_t length) {
  auto &&file = *static_cast<std::ifstream *>(png_get_io_ptr(png_ptr));
  file.read(reinterpret_cast<char *>(data), length);
}

static void write_fn(png_structp png_ptr, png_bytep data, png_size_t length) {
  auto &&file = *static_cast<std::ofstream *>(png_get_io_ptr(png_ptr));
  file.write(reinterpret_cast<char *>(data), length);
}

static void flush_fn(png_structp png_ptr) {
  auto &&file = *static_cast<std::ofstream *>(png_get_io_ptr(png_ptr));
  file.flush();
}
} // namespace

namespace PNG {
IStream::IStream(const char *filename)
    : file(filename, std::ios::in | std::ios::binary) {
  if (!file)
    throw std::runtime_error(std::string("unable to open '") + filename + "'");

  png_ptr = png_create_read_struct(PNG_LIBPNG_VER_STRING, nullptr, &error_fn,
                                   &warning_fn);
  png_set_read_fn(png_ptr, &file, &read_fn);

  if (png_ptr == nullptr)
    throw std::runtime_error("png_create_read_struct() failed");

  png_set_read_fn(png_ptr, &file, read_fn);

  info_ptr = png_create_info_struct(png_ptr);

  if (!info_ptr)
    throw std::runtime_error("png_create_info_struct() failed");

  png_read_info(png_ptr, info_ptr);
}

IStream::~IStream() { png_destroy_read_struct(&png_ptr, &info_ptr, nullptr); }

OStream::OStream(const char *filename)
    : file(filename, std::ios::out | std::ios::binary) {
  if (!file)
    throw std::runtime_error(std::string("unable to open '") + filename + "'");

  png_ptr = png_create_write_struct(PNG_LIBPNG_VER_STRING, nullptr, &error_fn,
                                    &warning_fn);

  if (png_ptr == nullptr)
    throw std::runtime_error("png_create_write_struct() failed");

  png_set_write_fn(png_ptr, &file, &write_fn, &flush_fn);

  info_ptr = png_create_info_struct(png_ptr);

  if (!info_ptr)
    throw std::runtime_error("png_create_info_struct() failed");
}

OStream::~OStream() { png_destroy_write_struct(&png_ptr, &info_ptr); }

RGBA8OStream::RGBA8OStream(const char *filename, size_t width, size_t height)
    : OStream(filename) {
  png_set_IHDR(*this, *this, static_cast<png_uint_32>(width),
               static_cast<png_uint_32>(height), 8, PNG_COLOR_TYPE_RGB_ALPHA,
               PNG_INTERLACE_NONE, PNG_COMPRESSION_TYPE_DEFAULT,
               PNG_FILTER_TYPE_DEFAULT);

  png_write_info(*this, *this);
}

RGBA8OStream::~RGBA8OStream() { png_write_end(*this, *this); }

void RGBA8OStream::writeRow(const std::uint32_t *row) {
  png_write_row(*this, reinterpret_cast<const png_byte *>(row));
}

std::tuple<int, int> readImageSize(const char *filename) {
  IStream file(filename);
  png_uint_32 w, h;
  int bit_depth, color_type, interlace_method, compression_method,
      filter_method;
  png_get_IHDR(file, file, &w, &h, &bit_depth, &color_type, &interlace_method,
               &compression_method, &filter_method);

  return std::tuple<int, int>(w, h);
}

image<std::uint32_t> loadImage2D(const char *filename) {
  IStream file(filename);

  png_uint_32 w, h;
  int bit_depth, color_type, interlace_method, compression_method,
      filter_method;
  png_get_IHDR(file, file, &w, &h, &bit_depth, &color_type, &interlace_method,
               &compression_method, &filter_method);

  if (bit_depth > 8)
    png_set_scale_16(file);
  else if (bit_depth < 8)
    png_set_packing(file);

  if (color_type == PNG_COLOR_TYPE_RGB) {
    png_set_add_alpha(file, 0xFF, PNG_FILLER_AFTER);
  } else if (color_type == PNG_COLOR_TYPE_GRAY ||
             color_type == PNG_COLOR_TYPE_GRAY_ALPHA) {
    png_set_gray_to_rgb(file);
    png_set_add_alpha(file, 0xFF, PNG_FILLER_AFTER);
  } else if (color_type == PNG_COLOR_TYPE_PALETTE) {
    png_set_palette_to_rgb(file);
    png_set_add_alpha(file, 0xFF, PNG_FILLER_AFTER);
  } else if (color_type == PNG_COLOR_TYPE_RGB_ALPHA)
    ;
  else
    throw std::runtime_error(std::string("color type not supported: '") +
                             filename + "'");

  png_read_update_info(file, file);

  image<std::uint32_t> img(w, h);

  std::unique_ptr<png_byte *[]> rows(new png_byte *[h]);
  for (png_uint_32 y = 0; y < h; ++y)
    rows[y] = reinterpret_cast<png_byte *>(data(img) + y * w);

  png_read_image(file, &rows[0]);

  png_read_end(file, file);

  return img;
}

void saveImage(const char *filename, const image<std::uint32_t> &img) {
  OStream file(filename);

  int w = static_cast<int>(width(img));
  int h = static_cast<int>(height(img));

  png_set_IHDR(file, file, w, h, 8, PNG_COLOR_TYPE_RGB_ALPHA,
               PNG_INTERLACE_NONE, PNG_COMPRESSION_TYPE_DEFAULT,
               PNG_FILTER_TYPE_DEFAULT);

  png_write_info(file, file);

  for (int y = 0; y < h; ++y)
    png_write_row(file, reinterpret_cast<const png_byte *>(data(img) + y * w));

  png_write_end(file, file);
}
} // namespace PNG
