#pragma once

#include <Robocode/Header.h>
#include <Robocode/DFT.h>

#include <qdft/qdft.h>

class QDFT final : public DFT, private qdft::QDFT<float, double>
{

public:

  QDFT(const double samplerate, const double concertpitch);
  ~QDFT() = default;

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
