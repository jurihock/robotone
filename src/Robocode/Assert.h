#pragma once

#include <cstdarg>
#include <filesystem>
#include <string>
#include <utility>

#define assert_true(condition, ...)  \
  do                                 \
  {                                  \
    assert_throw_if(                 \
      !(condition),                  \
      (#condition),                  \
      __FILE__,                      \
      __LINE__,                      \
      __VA_ARGS__);                  \
  }                                  \
  while(0)                          //

#define assert_false(condition, ...) \
  do                                 \
  {                                  \
    assert_throw_if(                 \
      (condition),                   \
      (#condition),                  \
      __FILE__,                      \
      __LINE__,                      \
      __VA_ARGS__);                  \
  }                                  \
  while(0)                          //

template<typename... Args>
static void assert_throw_if(
  const bool condition,
  const char* expression,
  const char* file,
  const int line,
  const char* message,
  Args&&... args)
{
  if (!condition)
  {
    return;
  }

  auto format = [](const char* format, ...)
  {
    va_list args;
    va_list temp;

    va_start(args, format);

    va_copy(temp, args);
    auto size = std::vsnprintf(nullptr, 0, format, temp);
    va_end(temp);

    std::string buffer(size + 1, '\0');
    std::vsnprintf(buffer.data(), buffer.size(), format, args);
    if (!buffer.empty()) { buffer.pop_back(); }

    va_end(args);

    return buffer;
  };

  auto filename = std::filesystem::path(file).filename();

  throw std::runtime_error(
    format(message, std::forward<Args>(args)...) +
    format(" (%s in %s at %d)", expression, filename.c_str(), line));
}
