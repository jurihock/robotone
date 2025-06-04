#pragma once

#include <juce_audio_processors/juce_audio_processors.h>
#include <juce_jingles/utils/Logger.h>

BEGIN_JUCE_JINGLES_NAMESPACE

template<typename Parameters, typename Editor>
class StereoProcessor : public juce::AudioProcessor
{

public:

  StereoProcessor() :
    juce::AudioProcessor(
      juce::AudioProcessor::BusesProperties()
        .withInput("Input",   juce::AudioChannelSet::stereo(), true)
        .withOutput("Output", juce::AudioChannelSet::stereo(), true)),
    parameters(std::make_unique<Parameters>(*this))
  {
  }

  virtual ~StereoProcessor() = default;

  virtual bool hasEditor() const override { return true; }
  virtual juce::AudioProcessorEditor* createEditor() override { return new Editor(*this); }

  virtual bool isMidiEffect() const override { return false; }
  virtual bool acceptsMidi() const override { return false; }
  virtual bool producesMidi() const override { return false; }
  virtual int  getNumPrograms() override { return 1; }
  virtual int  getCurrentProgram() override { return 0; }
  virtual void setCurrentProgram(int index) override { juce::ignoreUnused(index); }
  virtual void changeProgramName(int index, const juce::String& name) override { juce::ignoreUnused(index, name); }
  virtual const juce::String getProgramName (int index) override { juce::ignoreUnused(index); return {}; }

  virtual juce::AudioProcessorParameter* getBypassParameter() const override { return parameters->get("bypass"); }
  virtual double getTailLengthSeconds() const override { return 0; }

  virtual bool isBusesLayoutSupported(const BusesLayout& layouts) const override
  {
    if (layouts.getMainInputChannelSet() != juce::AudioChannelSet::mono() &&
        layouts.getMainInputChannelSet() != juce::AudioChannelSet::stereo())
      return false;

    if (layouts.getMainOutputChannelSet() != juce::AudioChannelSet::mono() &&
        layouts.getMainOutputChannelSet() != juce::AudioChannelSet::stereo())
      return false;

    return true;
  }

  virtual const juce::String getName() const override
  {
    return juce::String::formatted(
      "%s v%s",
      JUCE_JINGLES_PROJECT,
      JUCE_JINGLES_VERSION);
  }

  virtual void getStateInformation(juce::MemoryBlock& data) override
  {
    LOG("Save plugin state");

    try
    {
      parameters->save(data, [](auto xml) { LOG(xml); });
    }
    catch(const std::exception& exception)
    {
      juce::ignoreUnused(exception);

      LOG(exception.what());
    }
  }

  virtual void setStateInformation(const void* data, int size) override
  {
    LOG("Load plugin state");

    try
    {
      parameters->load(data, size, [](auto xml) { LOG(xml); });
    }
    catch(const std::exception& exception)
    {
      juce::ignoreUnused(exception);

      LOG(exception.what());
    }
  }

  virtual void releaseResources() override = 0;
  virtual void prepareToPlay(double samplerate, int blocksize) override = 0;
  virtual void processBlock(juce::AudioBuffer<float>& audio, juce::MidiBuffer& midi) override = 0;

protected:

  const std::unique_ptr<Parameters> parameters;

private:

  JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR(StereoProcessor)

};

END_JUCE_JINGLES_NAMESPACE
