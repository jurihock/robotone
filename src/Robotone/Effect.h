#pragma once

#include <JuceHeader.h>

// TODO #include <Robotone/Robotone.h>

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

  Effect(const double samplerate, const int downsampling = 2, const double concertpitch = 440);
  virtual ~Effect() = default;

  int latency() const override;

  void update(int note, double velocity);

  void dry(const std::span<const float> input, const std::span<float> output) override;
  void wet(const std::span<const float> input, const std::span<float> output) override;

private:

  struct
  {
    uint64_t sample;
    double samplerate;
    double downsampling;
    double concertpitch;
  } config;

  struct note_t
  {
    double omega;
    double velocity;
  };

  // TODO std::unique_ptr<Robotone> robotone;

  std::unique_ptr<SDFT<float, double>> sdft;
  std::vector<std::complex<double>> dft;

  std::array<note_t, 128> notes;
  std::vector<size_t> mask;

  JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR(Effect)

};
