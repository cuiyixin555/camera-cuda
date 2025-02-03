

#ifndef INCLUDED_UTILS_CMD_ARGS
#define INCLUDED_UTILS_CMD_ARGS

#pragma once

#include <cstring>
#include <stdexcept>

struct usage_error : std::runtime_error {
  explicit usage_error(const std::string &msg) : runtime_error(msg) {}
};

char **parseArgument(char **argv, size_t token_offset, int &v);
char **parseArgument(char **argv, size_t token_offset, float &v);

template <int S, typename T>
bool checkArgument(const char (&token)[S], char **&a, T &v) {
  if (std::strncmp(token, *a, strlen(token)) == 0) {
    a = parseArgument(a, strlen(token), v);
    return true;
  }
  return false;
}

template <int S>
bool checkArgument(const char (&token)[S], char **&a, bool &set) {
  if (std::strncmp(token, *a, strlen(token)) == 0) {
    set = true;
    return true;
  }
  return false;
}

#endif // INCLUDED_UTILS_CMD_ARGS
