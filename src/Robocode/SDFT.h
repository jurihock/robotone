#pragma once

#include <Robocode/Header.h>

#include <sdft/sdft.h>

class SDFT final : public sdft::SDFT<float, double>
{

public:

  SDFT(const double samplerate, const size_t dftsize);

  float transform(const float x, const std::function<void(
                  const std::span<const std::complex<double>> dftanal,
                  const std::span<std::complex<double>> dftsynth)> callback);

private:

  std::vector<std::complex<double>> input;
  std::vector<std::complex<double>> output;

};
