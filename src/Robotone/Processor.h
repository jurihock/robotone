#pragma once

#include <JuceHeader.h>

#include <Robotone/Editor.h>
#include <Robotone/Effect.h>
#include <Robotone/Parameters.h>

class Processor final : public juce::jingles::StereoProcessor<Parameters, Editor>
{

public:

  Processor();
  virtual ~Processor() = default;

  bool acceptsMidi() const override;
  void releaseResources() override;
  void prepareToPlay(double samplerate, int blocksize) override;
  void processBlock(juce::AudioBuffer<float>& audio, juce::MidiBuffer& midi) override;

private:

  struct Config
  {
    double samplerate;
    int blocksize;
  };

  std::mutex mutex;
  std::optional<Config> config;
  std::optional<bool> bypass;
  std::vector<std::unique_ptr<Effect>> effects;

  JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR(Processor)

};
