#pragma once

#include <JuceHeader.h>

#include <Robotone/Editor.h>
#include <Robotone/Effect.h>
#include <Robotone/Parameters.h>

class Processor final : public juce::jingles::AudioEffectProcessor<1, Effect, Parameters, Editor>
{

public:

  Processor();
  virtual ~Processor() = default;

  std::unique_ptr<Effect> createEffect(const size_t channel, const double samplerate, const size_t blocksize) override;

  bool acceptsMidi() const override;
  void processMidi(const juce::MidiBuffer& midi) override;

private:

  JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR(Processor)

};
