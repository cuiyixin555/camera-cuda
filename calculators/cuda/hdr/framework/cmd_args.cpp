

#include <cstdlib>
#include <cstring>

#include "cmd_args.h"

namespace {
template <std::size_t N> std::size_t strlen(const char (&str)[N]) {
  return N - 1;
}
} // namespace

char **parseArgument(char **argv, size_t token_offset, int &v) {
  const char *startptr = *argv + token_offset;
  char *argend = *argv + std::strlen(*argv);

  if (startptr == argend) {
    startptr = *++argv;
    argend = *argv + std::strlen(*argv);
  }

  char *endptr = nullptr;
  v = std::strtol(startptr, &endptr, 10);

  if (endptr < argend)
    throw usage_error("expected integer argument");

  return argv;
}

char **parseArgument(char **argv, size_t token_offset, float &v) {
  const char *startptr = *argv + token_offset;
  char *argend = *argv + std::strlen(*argv);

  if (startptr == argend) {
    startptr = *++argv;
    argend = *argv + std::strlen(*argv);
  }

  char *endptr = nullptr;
  v = std::strtof(startptr, &endptr);

  if (endptr < argend)
    throw usage_error("expected float argument");

  return argv;
}
