#include <Robocode/Channel.h>

Channel::Channel(const size_t index, const double samplerate, const std::span<const double> frequencies, const double concertpitch)
{
  const size_t dftsize = frequencies.size();

  config.dftsize = dftsize;

  config.freq = midi_to_hertz(index, concertpitch);
  config.gain = 0;

  config.samplerate = samplerate;
  config.concertpitch = concertpitch;

  config.chnfreqs.resize(dftsize);
  config.phase[0].resize(dftsize);
  config.phase[1].resize(dftsize);

  for (size_t i = 0; i < dftsize; ++i)
  {
    config.chnfreqs[i] = config.freq * frequencies[i];
  }
}

Channel::Channel(const Channel& other) :
  config(other.config)
{
}

Channel& Channel::operator=(const Channel& other)
{
  if (this != &other)
  {
    config = other.config;
  }

  return *this;
}

double Channel::gain() const
{
  return config.gain;
}

void Channel::gain(const double gain)
{
  config.gain = gain;
}

double Channel::synthesize(const std::span<std::complex<double>> dft,
                           const std::span<const double> dftfreqs,
                           const std::span<const double> pvcfreqs,
                           const int gestalt)
{
  assert_true(dft.size() == config.dftsize, "Invalid DFT size!");

  const double hz2rad = hertz_to_radian(config.samplerate);

  const double gain = config.gain;
  const auto& chnfreqs = config.chnfreqs;

  auto robotize = [&]()
  {
    auto& phase = config.phase[0];

    for (size_t i = 1; i < dft.size() - 1; ++i)
    {
      const double newfreq = chnfreqs[i];

      phase[i] += newfreq * hz2rad;

      if (newfreq <= dftfreqs.front())
      {
        continue;
      }

      if (newfreq >= dftfreqs.back())
      {
        continue;
      }

      dft[i] += std::polar(gain, phase[i]);
    }
  };

  auto humanize = [&]()
  {
    auto& phase = config.phase[1];

    for (size_t i = 1; i < dft.size() - 1; ++i)
    {
      const double newfreq = chnfreqs[i] * pvcfreqs[i];

      phase[i] += newfreq * hz2rad;

      if (newfreq <= dftfreqs.front())
      {
        continue;
      }

      if (newfreq >= dftfreqs.back())
      {
        continue;
      }

      dft[i] += std::polar(gain, phase[i]);
    }
  };

  if (gestalt < 0)
  {
    robotize();
    return gain;
  }
  else if (gestalt > 0)
  {
    humanize();
    return gain;
  }
  else
  {
    robotize();
    humanize();
    return gain + gain;
  }
}
