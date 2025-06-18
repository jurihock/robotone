#include <Robocode/Channels.h>

Channels::Channels(const double samplerate, const std::span<const double> frequencies, const double concertpitch)
{
  for (size_t i = 0; i < channels.size(); ++i)
  {
    channels[i] = Channel(i, samplerate, frequencies, concertpitch);
  }

  mask.reserve(channels.size());
}

void Channels::synthesize(const std::span<const std::complex<double>> dftanal,
                          const std::span<std::complex<double>> dftsynth,
                          const std::span<const double> dftfreqs,
                          const std::span<const double> pvcfreqs,
                          const int gestalt)
{
  double sum = 0;

  for (size_t channel : mask)
  {
    sum += channels[channel].synthesize(dftsynth, dftfreqs, pvcfreqs, gestalt);
  }

  for (size_t i = 1; i < dftfreqs.size() - 1; ++i)
  {
    const double abs = std::abs(dftanal[i]);

    dftsynth[i] = (sum > 0) ? (abs / sum) * dftsynth[i] : 0;
  }
}

void Channels::update(const int channel, const double gain)
{
  if (channel < 0 || static_cast<int>(channels.size()) <= channel)
  {
    return;
  }

  if (gain <= 0)
  {
    mask.erase(std::remove(mask.begin(), mask.end(), channel), mask.end());
  }
  else if (channels[channel].gain() <= 0)
  {
    mask.push_back(channel);
  }

  channels[channel].gain(gain);
}
