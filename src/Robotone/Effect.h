#pragma once

#include <JuceHeader.h>

#include <Robocode/Channels.h>
#include <Robocode/Noise.h>
#include <Robocode/QDFT.h>
#include <Robocode/SDFT.h>
#include <Robocode/SRC.h>
#include <Robocode/Vocoder.h>

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
  void gestalt(int value);

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
    int gestalt;
  };

  config_t config;

  std::unique_ptr<SRC> src;
  std::unique_ptr<DFT> dft;
  std::unique_ptr<Vocoder> vocoder;
  std::unique_ptr<Channels> channels;

  JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR(Effect)

};
