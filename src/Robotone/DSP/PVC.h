#pragma once

#include <algorithm>
#include <array>
#include <cmath>
#include <cstdint>
#include <functional>
#include <memory>
#include <numbers>
#include <numeric>
#include <span>
#include <vector>

class PVC final
{

public:

  PVC(const double samplerate, const std::vector<double>& frequencies) :
    samplerate(samplerate),
    frequencies(frequencies)
  {
    const size_t dftsize = frequencies.size();

    cache.resize(dftsize);
    omega.resize(dftsize);
    error.resize(dftsize);
    phase.resize(dftsize);
    freqs.resize(dftsize);

    for (size_t i = 0; i < dftsize; ++i)
    {
      omega[i] = 2 * std::numbers::pi * frequencies[i] / samplerate;
    }
  }

  void process(const std::span<std::complex<double>> dft,
    const std::function<void(const std::span<double> freqs)> callback)
  {
    const double a = samplerate / (2 * std::numbers::pi);
    const double s = (2 * std::numbers::pi) / samplerate;

    for (size_t i = 0; i < dft.size(); ++i)
    {
      error[i] = std::arg(dft[i] / cache[i]) - omega[i]; // TODO: approx, wrap
      freqs[i] = frequencies[i] + error[i] * a;
      cache[i] = dft[i];
    }

    callback({freqs.data(), freqs.size()});

    for (size_t i = 1; i < dft.size() - 1; ++i)
    {
      phase[i] += omega[i] + (freqs[i] - frequencies[i]) * s;

      if (freqs[i] <= frequencies.front())
      {
        dft[i] = 0;
        continue;
      }

      if (freqs[i] >= frequencies.back())
      {
        dft[i] = 0;
        continue;
      }

      const double abs = std::abs(dft[i]);
      const double arg = phase[i];

      dft[i] = std::polar(abs, arg);
    }

    dft[0] = dft[dft.size() - 1] = 0;
  }

private:

  const double samplerate;
  const std::vector<double> frequencies;

  std::vector<std::complex<double>> cache;
  std::vector<double> omega;
  std::vector<double> error;
  std::vector<double> phase;
  std::vector<double> freqs;

};
