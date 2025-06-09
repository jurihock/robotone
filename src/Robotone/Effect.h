#pragma once

#include <JuceHeader.h>

#include <Robotone/DSP/Noise.h>
#include <Robotone/DSP/SampleRateConverter.h>
#include <Robotone/DSP/SDFT.h>

#include <algorithm>
#include <array>
#include <cmath>
#include <cstdint>
#include <memory>
#include <numbers>
#include <span>
#include <vector>

class Effect final : public juce::jingles::AudioEffect
{

public:

  Effect(const double samplerate, const size_t blocksize, const double concertpitch = 440);
  virtual ~Effect() = default;

  int latency() const override;

  void reset();

  void decimate(bool value);
  void millis(int value);
  void octave(int value);

  void update(int note, double velocity);

  void dry(const std::span<const float> input, const std::span<float> output) override;
  void wet(const std::span<const float> input, const std::span<float> output) override;

private:

  struct config_t
  {
    double samplerate;
    size_t blocksize;
    double concertpitch;
    bool decimate;
    int millis;
    int octave;
  };

  struct note_t
  {
    double omega;
    double velocity;
  };

  config_t config;
  uint64_t sample;

  Noise<float> noise;
  std::unique_ptr<SampleRateConverter> src;

  std::unique_ptr<SDFT<float, double>> sdft;
  std::vector<std::complex<double>> dft;

  std::array<note_t, 128> notes;
  std::vector<size_t> mask;

  JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR(Effect)

};
