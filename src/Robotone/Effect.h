#pragma once

#include <JuceHeader.h>

#include <Robotone/SDFT/msdft.h>
// #include <Robotone/SDFT/osdft.h>
using namespace sdft;

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

  Effect(const double samplerate, const double concertpitch = 440);
  virtual ~Effect() = default;

  int latency() const override;

  void reset();

  void milliseconds(int value);
  void decimation(int value);

  void update(int note, double velocity);

  void dry(const std::span<const float> input, const std::span<float> output) override;
  void wet(const std::span<const float> input, const std::span<float> output) override;

private:

  struct config_t
  {
    uint64_t sample;
    double samplerate;
    double concertpitch;
    int milliseconds;
    int decimation;
  };

  struct note_t
  {
    double omega;
    double velocity;
  };

  config_t config;

  std::unique_ptr<SDFT<float, double>> sdft;
  std::vector<std::complex<double>> dft;

  std::array<note_t, 128> notes;
  std::vector<size_t> mask;

  JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR(Effect)

};
