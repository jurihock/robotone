#pragma once

#include <Robocode/Header.h>
#include <Robocode/Channel.h>

class Channels final
{

public:

  Channels(const double samplerate, const std::vector<double>& frequencies, const double concertpitch);

  void synthesize(const std::span<const std::complex<double>> dftanal,
                  const std::span<std::complex<double>> dftsynth,
                  const std::span<const double> pvcfreqs,
                  const int gestalt);

  void update(const int channel, const double gain);

private:

  const std::vector<double> frequencies;

  std::array<Channel, 128> channels;
  std::vector<size_t> mask;

};
