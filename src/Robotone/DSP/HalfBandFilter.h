#pragma once

#include <cmath>
#include <deque>
#include <limits>
#include <numbers>
#include <numeric>
#include <sstream>
#include <vector>

namespace dsp
{
  template<typename T>
  std::vector<T> halfband(const size_t taps = 19, const T beta = 6);
}

template<typename T, typename F>
class HalfBandFilter final
{

public:

    HalfBandFilter() :
      coeffs(dsp::halfband<F>()),
      buffer(coeffs.size(), 0)
    {}

    T filter(T x)
    {
      buffer.pop_back();
      buffer.push_front(static_cast<F>(x));

      const F y = std::transform_reduce(
        buffer.begin(),
        buffer.end(),
        coeffs.begin(),
        F(0));

      return static_cast<T>(y);
    }

    std::string str() const
    {
      std::ostringstream str;

      str << "[";

      for (size_t i = 0; i < coeffs.size(); ++i)
      {
        if (i)
        {
          str << ", ";
        }

        str << coeffs.at(i);
      }

      str << "]";

      return str.str();
    }

private:

    const std::vector<F> coeffs;
    std::deque<F> buffer;

};

namespace dsp
{
  template<typename T>
  T bessel(const T x)
  {
    const double z = std::abs(x);

    double y = 0;

    if (z < 3.75)
    {
      y = (x / 3.75);

      y *= y;

      y = 1.0 +
        y * (3.5156229 +
        y * (3.0899424 +
        y * (1.2067492 +
        y * (0.2659732 +
        y * (0.0360768 +
        y * (0.0045813))))));
    }
    else
    {
      y = 3.75 / z;

      y = (std::exp(z) / std::sqrt(z))
          * (+0.39894228 +
        y * (+0.01328592 +
        y * (+0.00225319 +
        y * (-0.00157565 +
        y * (+0.00916281 +
        y * (-0.02057706 +
        y * (+0.02635537 +
        y * (-0.01647633 +
        y * (+0.00392377)))))))));
    }

    return static_cast<T>(y);
  }

  template<typename T>
  std::vector<T> kaiser(const size_t taps, const T beta)
  {
    std::vector<T> window(taps);

    const double N = taps - 1.0;
    const double denom = bessel<double>(beta);

    for (size_t n = 0; n < taps; ++n)
    {
      const double arg = beta * std::sqrt(
        1.0 - std::pow((2.0 * n - N) / N, 2.0));

      window.at(n) = static_cast<T>(
        bessel<double>(arg) / denom);
    }

    return window;
  }

  template<typename T>
  std::vector<T> halfband(const size_t taps, const T beta)
  {
    const double N = taps - 1.0;

    std::vector<double> n;
    for (double i = -N/2; i <= N/2; ++i)
    {
      n.push_back(i);
    }

    const auto win = dsp::kaiser<double>(taps, beta);
    const auto eps = std::numeric_limits<double>::epsilon();

    std::vector<double> sinc(taps);
    for (size_t i = 0; i < taps; ++i)
    {
      if (n.at(i) == 0)
      {
        sinc.at(i) = 0.5;
      }
      else
      {
        sinc.at(i) = std::sin(n.at(i) * std::numbers::pi / 2) /
          (n.at(i) * std::numbers::pi + eps);
      }
    }

    std::vector<T> coeffs(taps);
    for (size_t i = 0; i < taps; ++i)
    {
      coeffs.at(i) = static_cast<T>(sinc.at(i) * win.at(i));
    }

    return coeffs;
  }
}
