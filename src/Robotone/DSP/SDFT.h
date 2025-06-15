#pragma once

#include <Robotone/DSP/Header.h>

#include <sdft/sdft.h>

class SDFT final : public sdft::SDFT<float, double>
{

public:

  SDFT(const double samplerate, const size_t dftsize) :
    sdft::SDFT<float, double>(samplerate, dftsize),
    input(dftsize),
    output(dftsize)
  {
  }

  float transform(const float x, const std::function<void(
    const std::span<const std::complex<double>> dftanal,
    const std::span<std::complex<double>> dftsynth)> callback)
  {
    sdft(x, input.data());

    std::fill(output.begin(), output.end(), std::complex<double>(0));
    callback({input.data(), input.size()}, {output.data(), output.size()});
    output[0] = output[output.size() - 1] = 0;

    float y = isdft(output.data());
    y = std::clamp<float>(y, -1, +1);

    return y;
  }

private:

  std::vector<std::complex<double>> input;
  std::vector<std::complex<double>> output;

};
