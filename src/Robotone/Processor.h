#pragma once

#include <JuceHeader.h>

#include <Robotone/Editor.h>
#include <Robotone/Effect.h>
#include <Robotone/Parameters.h>

class Processor final : public juce::jingles::AudioEffectProcessor<Parameters, Editor, Effect, 1>
{

public:

  Processor();
  virtual ~Processor() = default;

  std::unique_ptr<Effect> createEffect(const int channel, const double samplerate, const int blocksize) override;

  bool acceptsMidi() const override;
  void processMidi(const juce::MidiBuffer& midi) override;

private:

  JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR(Processor)

};
