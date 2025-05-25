#include <Robotone/Effect.h>

Effect::Effect(const double samplerate, const int downsampling, const double concertpitch) :
  config({
    .sample = 0,
    .samplerate = samplerate,
    .downsampling = downsampling > 1 ? 1 / double(downsampling) : 1,
    .concertpitch = concertpitch,
  })
{
  // TODO robotone = std::make_unique<Robotone>(samplerate);

  const size_t dftsize = static_cast<size_t>(256 * config.downsampling); // TODO dftsize

  sdft = std::make_unique<SDFT<float, double>>(dftsize);
  dft.resize(sdft->size());

  for (size_t i = 0; i < notes.size(); ++i)
  {
    const double sr = config.samplerate;
    const double cp = config.concertpitch;

    // https://newt.phys.unsw.edu.au/jw/notes.html
    const double hz = std::pow(2, (double(i) - 69) / 12) * cp;

    notes[i] =
    {
      .omega = 2 * std::numbers::pi * hz / sr,
      .velocity = 0,
    };
  }

  mask.reserve(notes.size());
}

int Effect::latency() const
{
  return 0; // TODO robotone->latency();
}

void Effect::update(int note, double velocity)
{
  if (note < 0 || notes.size() <= note)
  {
    return;
  }

  if (velocity <= 0)
  {
    mask.erase(std::remove(mask.begin(), mask.end(), note), mask.end());
  }
  else if (notes[note].velocity <= 0)
  {
    mask.push_back(note);
  }

  notes[note].velocity = velocity;
}

void Effect::dry(const std::span<const float> input, const std::span<float> output)
{
  std::transform(
    input.begin(),
    input.end(),
    output.begin(),
    [&](const float x)
    {
      return x; // TODO latency
    });
}

void Effect::wet(const std::span<const float> input, const std::span<float> output)
{
  std::transform(
    input.begin(),
    input.end(),
    output.begin(),
    [&](const float x)
    {
      sdft->sdft(x, dft.data());

      for (size_t i = 1; i < dft.size() - 1; ++i)
      {
        const double abs = std::abs(dft[i]);
        const double inc = double(i) * config.sample;
        std::complex<double> arg = 0;
        double sum = 0;

        for (const size_t j : mask)
        {
          const note_t& note = notes[j];

          arg += std::polar(note.velocity, note.omega * inc);
          sum += note.velocity;
        }

        dft[i] = sum > 0 ? (abs / sum) * arg : 0;
      }

      const float y = sdft->isdft(dft.data());

      ++config.sample;

      return y;
    });
}
