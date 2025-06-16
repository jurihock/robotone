#pragma once

#include <Robocode/Header.h>

class Vocoder final
{

public:

  Vocoder(const double samplerate, const std::vector<double>& frequencies);

  void analyze(const std::span<const std::complex<double>> dft,
               const std::function<void(const std::span<const double> pvcfreqs)> callback);

private:

  const double samplerate;
  const size_t dftsize;

  std::vector<std::complex<double>> cache;
  std::vector<double> freqs;

};
