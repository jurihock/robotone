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
  samplerate(samplerate),
  frequencies(frequencies)
{
  const size_t dftsize = frequencies.size();

  cache.resize(dftsize);
  freqs.resize(dftsize);
}

void Vocoder::analyze(const std::span<const std::complex<double>> dft,
                      const std::function<void(const std::span<const double> pvcfreqs)> callback)
{
  const double tofreq = samplerate / (2 * std::numbers::pi);

  for (size_t i = 0; i < dft.size(); ++i)
  {
    const double phase = angle(dft[i] / cache[i]); // TODO: approx, wrap

    freqs[i] = phase * tofreq;
    cache[i] = dft[i];
  }

  callback({freqs.data(), freqs.size()});
}
