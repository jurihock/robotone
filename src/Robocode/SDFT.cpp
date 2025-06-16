#include <Robocode/SDFT.h>

SDFT::SDFT(const double samplerate, const size_t dftsize) :
  sdft::SDFT<float, double>(samplerate, dftsize),
  input(dftsize),
  output(dftsize)
{
  assert_true(dftsize > 0, "Invalid DFT size!");
}

float SDFT::transform(const float x, const std::function<void(
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
