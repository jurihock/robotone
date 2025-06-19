#pragma once

#include <algorithm>
#include <cmath>
#include <cstdint>
#include <iterator>
#include <optional>
#include <span>

class Lerp final
{

public:

  Lerp(){};
  Lerp(const double v, const std::span<const double> x, const std::optional<double> z = std::nullopt);
  Lerp(const Lerp& other);
  Lerp& operator=(const Lerp& other);

  double operator()(const std::span<const double> y) const;

private:

  struct
  {
    ptrdiff_t i, j;
    size_t s;
    double t;
    int u;
    std::optional<double> z;
  }
  config;

};
