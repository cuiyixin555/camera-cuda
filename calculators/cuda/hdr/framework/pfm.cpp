

#include <fstream>
#include <string>


#include "calculators/cuda/hdr/framework/io.h"

#include "calculators/cuda/hdr/framework/pfm.h"

namespace {
template <typename T> image<T> load(std::istream &file, const char *type) {
  std::string magic;
  size_t w;
  size_t h;
  float a;
  file >> magic >> w >> h >> a;

  if (magic != type || a > 0.0f || file.get() != '\n')
    throw std::runtime_error("unsupported file format");

  image<T> img(w, h);

  for (size_t j = 0; j < h; ++j)
    read(file, data(img) + w * (h - 1 - j), w);

  return img;
}

template <typename T>
std::ostream &save(std::ostream &file, const image<T> &img, const char *type) {
  auto w = width(img);
  auto h = height(img);

  file << type << '\n' << w << ' ' << h << '\n' << -1.0f << '\n';

  for (size_t j = 0; j < h; ++j)
    write(file, data(img) + w * (h - j - 1), w);

  return file;
}
} // namespace

namespace PFM {
image<float> loadR32F(const char *filename) {
  std::ifstream file(filename, std::ios::binary | std::ios::in);
  return ::load<float>(file, "Pf");
}

void saveR32F(const char *filename, const image<float> &img) {
  std::ofstream file(filename, std::ios::binary | std::ios::out);
  ::save(file, img, "Pf");
}

image<RGB32F> loadRGB32F(const char *filename) {
  std::ifstream file(filename, std::ios::binary | std::ios::in);
  return ::load<RGB32F>(file, "PF");
}

void saveRGB32F(const char *filename, const image<RGB32F> &img) {
  std::ofstream file(filename, std::ios::binary | std::ios::out);
  ::save(file, img, "PF");
}
} // namespace PFM
