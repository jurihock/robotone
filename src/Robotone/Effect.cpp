#include <Robotone/Effect.h>

Effect::Effect(const double samplerate, const size_t blocksize, const double concertpitch) :
  config({
    .samplerate = samplerate,
    .blocksize = blocksize,
    .concertpitch = concertpitch,
    .decimate = false,
    .millis = 10,
    .octave = 0,
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
  const double sr = config.samplerate * (config.decimate ? 0.5 : 1);
  const double cp = config.concertpitch;

  const double window = std::max(double(config.millis), 1.0) * sr * 1e-3;
  const double factor = std::pow(2.0, double(config.octave)) * 0.5;
  const size_t dftsize = static_cast<size_t>(std::max(window * factor, 1.0));

  LOG("Reset millis %d octave %d window %f factor %f dftsize %zu",
    config.millis, config.octave, window, factor, dftsize);

  src = std::make_unique<SRC>(config.samplerate, sr, config.blocksize);
  sdft = std::make_unique<SDFT<float, double>>(dftsize);

  for (size_t i = 0; i < notes.size(); ++i)
  {
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

void Effect::decimate(bool value)
{
  config.decimate = value;
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
  const auto process = [this](const float x, const uint64_t sample) -> float
  {
    sdft->sdft(x, dft.data());

    for (size_t i = 1; i < dft.size() - 1; ++i)
    {
      const double abs = std::abs(dft[i]);
      const double inc = double(i) * sample;

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

    return y;
  };

  src->resample(input, output,
    [process, this](const std::span<const float> input,
                    const std::span<float> output)
  {
    std::transform(
      input.begin(),
      input.end(),
      output.begin(),
      [&](float x)
      {
        // x = noise();

        float y = process(x, sample++);

        return y;
      });
  });
}
