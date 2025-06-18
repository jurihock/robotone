#pragma once

#include <Robocode/Header.h>

class DFT
{

public:

  virtual ~DFT() = default;

  virtual size_t size() const = 0;
  virtual double samplerate() const = 0;
  virtual std::span<const double> frequencies(const bool normalize = false) const = 0;

  virtual float transform(const float x, const std::function<void(
                          const std::span<const std::complex<double>> dftanal,
                          const std::span<std::complex<double>> dftsynth,
                          const std::span<const double> dftfreqs)> callback) = 0;

};
