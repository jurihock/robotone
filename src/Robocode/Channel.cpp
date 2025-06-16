#include <Robocode/Channel.h>

Channel::Channel()
{
}

Channel::Channel(const size_t index, const size_t dftsize, const double samplerate, const double concertpitch)
{
  // https://newt.phys.unsw.edu.au/jw/notes.html
  const double hz = std::pow(2, (double(index) - 69) / 12) * concertpitch;

  config.freq = hz;
  config.gain = 0;

  config.samplerate = samplerate;
  config.concertpitch = concertpitch;

  config.freqs.resize(dftsize);
  config.phase.resize(dftsize);
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
                           const double gestalt) // TODO: gestalt
{
  const double tophase = (2 * std::numbers::pi) / config.samplerate;

  const double freq = config.freq;
  const double gain = config.gain;

  auto& freqs = config.freqs;
  auto& phase = config.phase;

  for (size_t i = 1; i < dft.size() - 1; ++i)
  {
    const double f0 = pvcfreqs[i];
    const double f1 = freq * i;

    freqs[i] = f1 * f0 / dftfreqs[i]; // TODO: f1 + (f0 - dftfreqs[i]) * (f1 / dftfreqs[i])
  }

  for (size_t i = 1; i < dft.size() - 1; ++i)
  {
    phase[i] += freqs[i] * tophase; // TODO: omega[i] + (freqs[i] - dftfreqs[i]) * tophase

    if (freqs[i] <= dftfreqs.front())
    {
      continue;
    }

    if (freqs[i] >= dftfreqs.back())
    {
      continue;
    }

    dft[i] += std::polar(gain, phase[i]);
  }

  return gain;
}
