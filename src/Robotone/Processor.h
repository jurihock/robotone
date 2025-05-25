#pragma once

#include <JuceHeader.h>

#include <Robotone/Editor.h>
#include <Robotone/Effect.h>
#include <Robotone/Parameters.h>

class Processor final : public juce::AudioProcessor
{

private:

  struct Config
  {
    double samplerate;
    int blocksize;
  };

public:

  Processor();

  const juce::String getName() const override;

  bool hasEditor() const override;
  juce::AudioProcessorEditor* createEditor() override;

  bool isMidiEffect() const override;
  bool acceptsMidi() const override;
  bool producesMidi() const override;
  int  getNumPrograms() override;
  int  getCurrentProgram() override;
  void setCurrentProgram(int index) override;
  void changeProgramName(int index, const juce::String& name) override;
  const juce::String getProgramName (int index) override;

  double getTailLengthSeconds() const override;
  juce::AudioProcessorParameter* getBypassParameter() const override;
  bool isBusesLayoutSupported(const BusesLayout& layouts) const override;

  void getStateInformation(juce::MemoryBlock& data) override;
  void setStateInformation(const void* data, int size) override;

  void prepareToPlay(double samplerate, int blocksize) override;
  void releaseResources() override;

  void processBlock(juce::AudioBuffer<float>& audio, juce::MidiBuffer& midi) override;

private:

  std::mutex mutex;
  std::optional<Config> config;
  std::optional<bool> bypass;
  std::vector<std::unique_ptr<Effect>> effects;
  std::unique_ptr<Parameters> parameters;

  JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR(Processor)

};
