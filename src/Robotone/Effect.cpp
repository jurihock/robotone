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
  sdft = std::make_unique<SDFT>(sr, dftsize);

  vocoder = std::make_unique<Vocoder>(sdft->samplerate(), sdft->frequencies());
  channels = std::make_unique<Channels>(sdft->samplerate(), sdft->frequencies(), cp);

  sample = 0;
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
  // const auto process1 = [this](const float x, const uint64_t sample) -> float
  // {
  //   sdft->sdft(x, dft.data());

  //   const double sum = std::accumulate(
  //     mask.begin(), mask.end(), double(0),
  //     [&](double res, size_t note)
  //     {
  //       return res + notes[note].velocity;
  //     });

  //   for (size_t i = 1; i < dft.size() - 1; ++i)
  //   {
  //     const double abs = std::abs(dft[i]);
  //     const double inc = static_cast<double>(sample * i);

  //     const std::complex<double> val = std::accumulate(
  //       mask.begin(), mask.end(), std::complex<double>(0),
  //       [&](std::complex<double> res, size_t note)
  //       {
  //         const auto& [hz, omg, vel] = notes[note];
  //         return res + std::polar(vel, omg * inc);
  //       });

  //     dft[i] = (sum > 0) ? (abs / sum) * val : 0;
  //   }

  //   dft[0] = dft[dft.size() - 1] = 0;

  //   const float y = sdft->isdft(dft.data());

  //   return y;
  // };

  // const auto process2 = [this](const float x, const uint64_t sample) -> float
  // {
  //   sdft->sdft(x, dft.data());

  //   pvc->process(dft, [&](const std::span<double> freqs)
  //   {
  //     const auto bins = sdft->frequencies();
  //     const double freq = (mask.size() > 0) ? notes[mask.front()].hz : 0;

  //     for (size_t i = 0; i < freqs.size(); ++i)
  //     {
  //       const double f0 = freqs[i];
  //       const double f1 = freq * i;

  //       freqs[i] = f1 * f0 / bins[i]; // f1 + (f0 - bins[i]) * (f1 / bins[i])
  //     }
  //   });

  //   const float y = sdft->isdft(dft.data());

  //   return y;
  // };

  // juce::ignoreUnused(process1);
  // juce::ignoreUnused(process2);

  // const auto process = process2;

  const auto process = [this](const float x, const uint64_t sample) -> float
  {
    return sdft->transform(x, [&](const std::span<const std::complex<double>> dftanal,
                                  const std::span<std::complex<double>> dftsynth)
    {
      vocoder->analyze(dftanal, [&](const std::span<const double> pvcfreqs)
      {
        channels->synthesize(dftanal, dftsynth, pvcfreqs);
      });
    });
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
