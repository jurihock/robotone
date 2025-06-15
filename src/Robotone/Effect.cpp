#include <Robotone/Effect.h>

Effect::Effect(const double samplerate, const size_t blocksize, const double concertpitch) :
  config({
    .samplerate = samplerate,
    .blocksize = blocksize,
    .concertpitch = concertpitch,
    .decimate = false,
    .millis = 10,
    .octave = 0,
    .gestalt = 0,
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
  sdft = std::make_unique<SDFT>(sr, dftsize);

  vocoder = std::make_unique<Vocoder>(sdft->samplerate(), sdft->frequencies());
  channels = std::make_unique<Channels>(sdft->samplerate(), sdft->frequencies(), cp);
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

void Effect::gestalt(double value)
{
  config.gestalt = value;
}

void Effect::update(int note, double velocity)
{
  channels->update(note, velocity);
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
  const double gestalt = config.gestalt;

  src->resample(input, output, [&](const std::span<const float> input,
                                   const std::span<float> output)
  {
    std::transform(
      input.begin(),
      input.end(),
      output.begin(),
      [&](float x)
      {
        // x = noise();

        float y = sdft->transform(x, [&](const std::span<const std::complex<double>> dftanal,
                                         const std::span<std::complex<double>> dftsynth)
        {
          vocoder->analyze(dftanal, [&](const std::span<const double> pvcfreqs)
          {
            channels->synthesize(dftanal, dftsynth, pvcfreqs, gestalt);
          });
        });

        return y;
      });
  });
}
