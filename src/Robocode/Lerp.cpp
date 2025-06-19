#include <Robocode/Lerp.h>

Lerp::Lerp(const double v, const std::span<const double> x, const std::optional<double> z)
{
  if (x.empty())
  {
    throw std::invalid_argument("Vector x must be non-empty!");
  }

  config.s = x.size();
  config.z = z;

  if (v <= x.front())
  {
    config.u = -1;
    return;
  }

  if (v >= x.back())
  {
    config.u = +1;
    return;
  }

  auto b = std::lower_bound(x.begin(), x.end(), v);

  if (b == x.begin())
  {
    config.u = -1;
    return;
  }

  if (b == x.end())
  {
    config.u = +1;
    return;
  }

  ptrdiff_t d = std::distance(x.begin(), b);
  ptrdiff_t i = d - 1;
  ptrdiff_t j = i + 1;

  double x0 = x[i];
  double x1 = x[j];
  double t = (v - x0) / (x1 - x0);

  config.i = i;
  config.j = j;
  config.t = t;
}

Lerp::Lerp(const Lerp& other) :
  config(other.config)
{
}

Lerp& Lerp::operator=(const Lerp& other)
{
  if (this != &other)
  {
    config = other.config;
  }

  return *this;
}

double Lerp::operator()(const std::span<const double> y) const
{
  const auto& [i, j, s, t, u, z] = config;

  if (y.size() != s)
  {
    throw std::invalid_argument("Vectors x and y must be the same size!");
  }

  if (u < 0)
  {
    return z.value_or(y.front());
  }
  else if (u > 0)
  {
    return z.value_or(y.back());
  }
  else
  {
    double y0 = y[i];
    double y1 = y[j];

    // y0 + t * (y1 - y0)
    return std::lerp(y0, y1, t);
  }
}
