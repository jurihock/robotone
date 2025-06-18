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
  const double cp = config.concertpitch * std::pow(2.0, double(config.octave));

  const double window = std::max(double(config.millis), 1.0) * sr * 1e-3;
  const size_t dftsize = static_cast<size_t>(std::max(window * 0.5, 1.0));

  LOG("Reset millis %d octave %d window %f dftsize %zu",
    config.millis, config.octave, window, dftsize);

  src = std::make_unique<SRC>(config.samplerate, sr, config.blocksize);

  dft = std::make_unique<SDFT>(sr, dftsize);
  // dft = std::make_unique<QDFT>(sr);

  vocoder = std::make_unique<Vocoder>(dft->samplerate(), dft->frequencies());
  channels = std::make_unique<Channels>(dft->samplerate(), dft->frequencies(true), cp);
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

void Effect::gestalt(int value)
{
  config.gestalt = value;
}

void Effect::update(int note, double velocity)
{
  channels->update(note, velocity);
}

void Effect::dry(const std::span<const float> input, const std::span<float> output)
{
  src->resample(input, output, [&](const std::span<const float> input,
                                   const std::span<float> output)
  {
    std::transform(
      input.begin(),
      input.end(),
      output.begin(),
      [&](float x)
      {
        float y = dft->transform(x, [&](const std::span<const std::complex<double>> dftanal,
                                        const std::span<std::complex<double>> dftsynth,
                                        const std::span<const double> dftfreqs)
        {
          std::copy(dftanal.begin(), dftanal.end(), dftsynth.begin());
        });

        return y;
      });
  });
}

void Effect::wet(const std::span<const float> input, const std::span<float> output)
{
  const int gestalt = config.gestalt;

  src->resample(input, output, [&](const std::span<const float> input,
                                   const std::span<float> output)
  {
    std::transform(
      input.begin(),
      input.end(),
      output.begin(),
      [&](float x)
      {
        float y = dft->transform(x, [&](const std::span<const std::complex<double>> dftanal,
                                        const std::span<std::complex<double>> dftsynth,
                                        const std::span<const double> dftfreqs)
        {
          vocoder->analyze(dftanal, [&](const std::span<const double> pvcfreqs)
          {
            channels->synthesize(dftanal, dftsynth, dftfreqs, pvcfreqs, gestalt);
          });
        });

        return y;
      });
  });
}
