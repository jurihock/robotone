#include <Robotone/Effect.h>

Effect::Effect(const double samplerate, const double concertpitch) :
  config({
    .sample = 0,
    .samplerate = samplerate,
    .concertpitch = concertpitch,
    .millis = 10,
    .octave = 0
  })
{
  reset();
}

int Effect::latency() const
{
  return 0; // TODO: latency
}

void Effect::reset()
{
  const double window = std::max(double(config.millis), 1.0) * config.samplerate * 1e-3;
  const double factor = std::pow(2.0, double(config.octave)) * 0.5;
  const size_t dftsize = static_cast<size_t>(std::max(window * factor, 1.0));

  LOG("Reset millis %d octave %d window %f factor %f dftsize %zu",
    config.millis, config.octave, window, factor, dftsize);

  sdft = std::make_unique<SDFT<float, double>>(dftsize);

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

  dft.resize(sdft->size());
  std::fill(dft.begin(), dft.end(), 0);

  mask.clear();
  mask.reserve(notes.size());
}

void Effect::millis(int value)
{
  config.millis = value;
}

void Effect::octave(int value)
{
  config.octave = value;
}

void Effect::update(int note, double velocity)
{
  if (note < 0 || static_cast<int>(notes.size()) <= note)
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
      return x; // TODO: latency
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
          const auto& [omg, vel] = notes[j];

          arg += std::polar(vel, omg * inc);
          sum += vel;
        }

        dft[i] = (sum > 0) ? (abs / sum) * arg : 0;
      }

      dft[0] = dft[dft.size() - 1] = 0;

      const float y = sdft->isdft(dft.data());

      ++config.sample;

      return y;
    });
}
