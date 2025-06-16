#include <Robocode/Channel.h>

Channel::Channel()
{
}

Channel::Channel(const size_t index, const size_t dftsize, const double samplerate, const double concertpitch)
{
  // https://newt.phys.unsw.edu.au/jw/notes.html
  const double hz = std::pow(2, (double(index) - 69) / 12) * concertpitch;

  config.dftsize = dftsize;

  config.freq = hz;
  config.gain = 0;

  config.samplerate = samplerate;
  config.concertpitch = concertpitch;

  config.chnfreqs.resize(dftsize);
  config.robotphase.resize(dftsize);
  config.humanphase.resize(dftsize);

  for (size_t i = 0; i < dftsize; ++i)
  {
    config.chnfreqs[i] = hz * i;
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
                           const double gestalt)
{
  assert_true(dft.size() == config.dftsize, "Invalid DFT size!");

  const double freq2phase = (2 * std::numbers::pi) / config.samplerate;

  const double human = (gestalt + 1) / 2;
  const double robot = 1 - human;

  const double gain = config.gain;
  const auto& chnfreqs = config.chnfreqs;

  auto& robotphase = config.robotphase;
  auto& humanphase = config.humanphase;

  for (size_t i = 1; i < dft.size() - 1; ++i)
  {
    const double chnfreq = chnfreqs[i];
    const double pvcfreq = pvcfreqs[i];

    const double robotfreq = chnfreq;
    const double humanfreq = chnfreq * pvcfreq;

    robotphase[i] += robotfreq * freq2phase;
    humanphase[i] += humanfreq * freq2phase;

    if (robotfreq <= dftfreqs.front())
    {
      continue;
    }

    if (robotfreq >= dftfreqs.back())
    {
      continue;
    }

    if (humanfreq <= dftfreqs.front())
    {
      continue;
    }

    if (humanfreq >= dftfreqs.back())
    {
      continue;
    }

    dft[i] += robot * std::polar(gain, robotphase[i]);
    dft[i] += human * std::polar(gain, humanphase[i]);
  }

  return gain;
}
