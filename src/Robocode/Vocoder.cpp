#include <Robocode/Vocoder.h>

Vocoder::Vocoder(const double samplerate, const std::span<const double> frequencies) :
  dftsize(frequencies.size())
{
  cache.resize(dftsize);
  slope.resize(dftsize);
  freqs.resize(dftsize);

  const double rad2hz = radian_to_hertz(samplerate);

  std::transform(
    frequencies.begin(),
    frequencies.end(),
    slope.begin(),
    [&](const double freq)
    {
      return (freq > 0) ? rad2hz / freq : 0;
    });
}

void Vocoder::analyze(const std::span<const std::complex<double>> dft,
                      const std::function<void(const std::span<const double> pvcfreqs)> callback)
{
  assert_true(dft.size() == dftsize, "Invalid DFT size!");

  for (size_t i = 0; i < dft.size(); ++i)
  {
    const double phase = angle(dft[i] / cache[i]); // TODO: wrap

    freqs[i] = phase * slope[i];
    cache[i] = dft[i];
  }

  callback({freqs.data(), freqs.size()});
}
