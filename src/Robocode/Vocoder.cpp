#include <Robocode/Vocoder.h>

#include <Robocode/Arctangent.h>

inline double angle(const std::complex<double>& z)
{
  #if defined(ENABLE_ARCTANGENT_APPROXIMATION)
    return Arctangent::atan2(z);
  #else
    return std::arg(z);
  #endif
}

Vocoder::Vocoder(const double samplerate, const std::vector<double>& frequencies) :
  dftsize(frequencies.size())
{
  cache.resize(dftsize);
  slope.resize(dftsize);
  freqs.resize(dftsize);

  const double phase2freq = samplerate / (2 * std::numbers::pi);

  for (size_t i = 0; i < dftsize; ++i)
  {
    slope[i] = phase2freq / frequencies[i];
  }
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
