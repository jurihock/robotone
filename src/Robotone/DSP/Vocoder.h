#pragma once

#include <Robotone/DSP/Header.h>

class Vocoder final
{

public:

  Vocoder(const double samplerate, const std::vector<double>& frequencies) :
    samplerate(samplerate),
    frequencies(frequencies)
  {
    const size_t dftsize = frequencies.size();

    cache.resize(dftsize);
    omega.resize(dftsize);
    error.resize(dftsize);
    freqs.resize(dftsize);

    for (size_t i = 0; i < dftsize; ++i)
    {
      omega[i] = 2 * std::numbers::pi * frequencies[i] / samplerate;
    }
  }

  void analyze(const std::span<const std::complex<double>> dft,
               const std::function<void(const std::span<const double> pvcfreqs)> callback)
  {
    const double tofreq = samplerate / (2 * std::numbers::pi);

    for (size_t i = 0; i < dft.size(); ++i)
    {
      error[i] = std::arg(dft[i] / cache[i]) - omega[i]; // TODO: approx, wrap
      freqs[i] = frequencies[i] + error[i] * tofreq;
      cache[i] = dft[i];
    }

    callback({freqs.data(), freqs.size()});
  }

private:

  const double samplerate;
  const std::vector<double> frequencies;

  std::vector<std::complex<double>> cache;
  std::vector<double> omega;
  std::vector<double> error;
  std::vector<double> freqs;

};
