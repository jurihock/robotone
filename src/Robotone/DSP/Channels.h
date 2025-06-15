#pragma once

#include <Robotone/DSP/Header.h>
#include <Robotone/DSP/Channel.h>

class Channels final
{

public:

  Channels(const double samplerate, const std::vector<double>& frequencies, const double concertpitch) :
    frequencies(frequencies)
  {
    for (size_t i = 0; i < channels.size(); ++i)
    {
      channels[i] = Channel(i, frequencies.size(), samplerate, concertpitch);
    }

    mask.reserve(channels.size());
  }

  void synthesize(const std::span<const std::complex<double>> dftanal,
                  const std::span<std::complex<double>> dftsynth,
                  const std::span<const double> pvcfreqs)
  {
    const std::span<const double> dftfreqs(frequencies.data(), frequencies.size());

    double sum = 0;

    for (size_t channel : mask)
    {
      sum += channels[channel].synthesize(dftsynth, dftfreqs, pvcfreqs);
    }

    for (size_t i = 1; i < dftsynth.size() - 1; ++i)
    {
      const double abs = std::abs(dftanal[i]);

      dftsynth[i] = (sum > 0) ? (abs / sum) * dftsynth[i] : 0;
    }
  }

  void update(const int channel, const double gain)
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

private:

  const std::vector<double> frequencies;

  std::array<Channel, 128> channels;
  std::vector<size_t> mask;

};
