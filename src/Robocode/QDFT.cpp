#include <Robocode/QDFT.h>

auto bw1 = std::pow(2, (double(021) - 69) / 12) * 440; // A0 ~ 27.5 Hz
auto bw2 = std::pow(2, (double(108) - 69) / 12) * 440; // C8 ~ 4186 Hz
auto bw = std::make_pair(bw1, bw2);

QDFT::QDFT(const double samplerate) :
  qdft::QDFT<float, double>(samplerate, bw),
  input(size()),
  output(size())
{
  assert_true(size() > 0, "Invalid DFT size!");

  const auto resolution = qdft::QDFT<float, double>::resolution();

  factors.resize(size());

  for (size_t i = 0; i < factors.size(); ++i)
  {
    factors[i] = std::pow(2.0, i / resolution);
  }
}

size_t QDFT::size() const
{
  return qdft::QDFT<float, double>::size();
}

double QDFT::samplerate() const
{
  return qdft::QDFT<float, double>::samplerate();
}

std::span<const double> QDFT::frequencies(const bool normalize) const
{
  if (normalize)
  {
    return factors;
  }

  return qdft::QDFT<float, double>::frequencies();
}

float QDFT::transform(const float x, const std::function<void(
                      const std::span<const std::complex<double>> dftanal,
                      const std::span<std::complex<double>> dftsynth,
                      const std::span<const double> dftfreqs)> callback)
{
  qdft(x, input.data());

  std::fill(output.begin(), output.end(), std::complex<double>(0));
  callback(input, output, frequencies());
  output[0] = output[output.size() - 1] = 0;

  float y = iqdft(output.data());
  y = std::clamp<float>(y, -1, +1);

  return y;
}
