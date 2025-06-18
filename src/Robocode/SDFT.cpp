#include <Robocode/SDFT.h>

SDFT::SDFT(const double samplerate, const size_t dftsize) :
  sdft::SDFT<float, double>(samplerate, dftsize),
  input(dftsize),
  output(dftsize)
{
  assert_true(dftsize > 0, "Invalid DFT size!");

  const auto hz = sdft::SDFT<float, double>::frequencies();
  const auto sr = sdft::SDFT<float, double>::samplerate();

  factors.resize(dftsize);

  for (size_t i = 0; i < dftsize; ++i)
  {
    factors[i] = hz[i] / sr;
  }
}

size_t SDFT::size() const
{
  return sdft::SDFT<float, double>::size();
}

double SDFT::samplerate() const
{
  return sdft::SDFT<float, double>::samplerate();
}

std::span<const double> SDFT::frequencies(const bool normalize) const
{
  if (normalize)
  {
    return factors;
  }

  return sdft::SDFT<float, double>::frequencies();
}

float SDFT::transform(const float x, const std::function<void(
                      const std::span<const std::complex<double>> dftanal,
                      const std::span<std::complex<double>> dftsynth,
                      const std::span<const double> dftfreqs)> callback)
{
  sdft(x, input.data());

  std::fill(output.begin(), output.end(), std::complex<double>(0));
  callback(input, output, frequencies());
  output[0] = output[output.size() - 1] = 0;

  float y = isdft(output.data());
  y = std::clamp<float>(y, -1, +1);

  return y;
}
