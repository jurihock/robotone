#pragma once

#include <Robocode/Header.h>
#include <Robocode/DFT.h>

#include <sdft/sdft.h>

class SDFT final : public DFT, private sdft::SDFT<float, double>
{

public:

  SDFT(const double samplerate, const size_t dftsize);

  size_t size() const override;
  double samplerate() const override;
  std::span<const double> frequencies(const bool normalize = false) const override;

  float transform(const float x, const std::function<void(
                  const std::span<const std::complex<double>> dftanal,
                  const std::span<std::complex<double>> dftsynth,
                  const std::span<const double> dftfreqs)> callback) override;

private:

  std::vector<double> factors;

  std::vector<std::complex<double>> input;
  std::vector<std::complex<double>> output;

};
