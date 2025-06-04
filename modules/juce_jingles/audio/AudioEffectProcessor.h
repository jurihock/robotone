#pragma once

#include <juce_audio_processors/juce_audio_processors.h>

#include <juce_jingles/audio/AudioEffect.h>
#include <juce_jingles/params/XmlParameters.h>

#include <juce_jingles/utils/Chronometer.h>
#include <juce_jingles/utils/Logger.h>

BEGIN_JUCE_JINGLES_NAMESPACE

template<typename Parameters, typename Editor, typename Effect, size_t Channels>
class AudioEffectProcessor : public juce::AudioProcessor
{

  static_assert(std::is_convertible<Parameters*, XmlParameters*>::value);
  static_assert(std::is_convertible<Editor*, juce::AudioProcessorEditor*>::value);
  static_assert(std::is_convertible<Effect*, AudioEffect*>::value);
  static_assert((Channels == 1) || (Channels == 2));

public:

  AudioEffectProcessor() :
    juce::AudioProcessor(
      juce::AudioProcessor::BusesProperties()
        .withInput("Input",   juce::AudioChannelSet::stereo(), true)
        .withOutput("Output", juce::AudioChannelSet::stereo(), true)),
    parameters(*this)
  {
    parameters.notify("bypass", [&](auto id, auto value)
    {
      std::lock_guard lock(mutex);
      bypass = std::get<bool>(value);
    });
  }

  virtual ~AudioEffectProcessor() = default;

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

  virtual juce::AudioProcessorParameter* getBypassParameter() const override { return parameters.get("bypass"); }
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
      parameters.save(data, [](auto xml) { LOG(xml); });
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
      parameters.load(data, size, [](auto xml) { LOG(xml); });
    }
    catch(const std::exception& exception)
    {
      juce::ignoreUnused(exception);

      LOG(exception.what());
    }
  }

  virtual void releaseResources() override
  {
    std::lock_guard lock(mutex);

    LOG("Release resources");

    config.reset();

    for (size_t i = 0; i < effects.size(); ++i)
    {
      effects.at(i).reset();
    }
  }

  virtual void prepareToPlay(double samplerate, int blocksize) override
  {
    std::lock_guard lock(mutex);

    LOG("Release resources");

    config.reset();

    for (size_t i = 0; i < effects.size(); ++i)
    {
      effects.at(i).reset();
    }

    if (samplerate < 1)
    {
      LOG("Prepare to play (invalid samplerate %g)", samplerate);
      return;
    }

    if (blocksize < 1)
    {
      LOG("Prepare to play (invalid blocksize %d)", blocksize);
      return;
    }

    LOG("Prepare to play (samplerate %g, blocksize %d)", samplerate, blocksize);

    config =
    {
      .samplerate = samplerate,
      .blocksize = blocksize
    };

    try
    {
      for (auto channel = 0; channel < effects.size(); ++channel) // TODO
      {
        auto effect = createEffect(channel, samplerate, blocksize);

        effects.at(channel) = std::move(effect);

        if (channel < 1)
        {
          const auto latency = effects.at(0)->latency();

          LOG("Latency %d (%d ms)", latency, static_cast<int>(1e+3 * latency / samplerate));

          setLatencySamples(latency);
        }
      }
    }
    catch(const std::exception& exception)
    {
      juce::ignoreUnused(exception);

      LOG(exception.what());
    }
  }

  virtual void processBlock(juce::AudioBuffer<float>& audio, juce::MidiBuffer& midi) override
  {
    juce::ScopedNoDenormals dontcare;

    std::lock_guard lock(mutex);

    const int effect_channels = static_cast<int>(effects.size());
    const int input_channels  = getTotalNumInputChannels();
    const int output_channels = getTotalNumOutputChannels();
    const int channel_samples = audio.getNumSamples();

    if (input_channels < 1)
    {
      LOG("Skip block (invalid number of input channels %d)", input_channels);
      return;
    }

    if (output_channels < 1)
    {
      LOG("Skip block (invalid number of output channels %d)", output_channels);
      return;
    }

    if (channel_samples < 1)
    {
      LOG("Skip block (invalid number of samples per block %d)", channel_samples);
      return;
    }

    const auto process_stereo_input = [&]() // TODO
    {
      for (int j = 0; j < std::min(output_channels, effect_channels); ++j)
      {
        int i = std::min(j, input_channels - 1);

        auto input = std::span<const float>(
          audio.getReadPointer(i),
          static_cast<size_t>(channel_samples));

        auto output = std::span<float>(
          audio.getWritePointer(j),
          static_cast<size_t>(channel_samples));

        if (bypass.value_or(parameters.bypass()))
        {
          effects.at(j)->dry(input, output);
        }
        else
        {
          effects.at(j)->wet(input, output);
        }
      }

      for (int j = effect_channels, i = effect_channels - 1; j < output_channels; ++j)
      {
        audio.copyFrom(j, 0, audio, i, 0, channel_samples);
      }
    };

    const auto process_stereo_output = [&](const std::string& error = "")
    {
      if (!error.empty())
      {
        LOG("Copy input to output (%s)", error.c_str());
      }

      for (int j = 1, i = 0; j < output_channels; ++j)
      {
        audio.copyFrom(j, 0, audio, i, 0, channel_samples);
      }
    };

    TIC();

    if (!config)
    {
      process_stereo_output("plugin is not prepared");
    }
    else if (!effects.size())
    {
      process_stereo_output("effects are not initialized");
    }
    else
    {
      try
      {
        processMidi(midi);
        process_stereo_input();
      }
      catch(const std::exception& exception)
      {
        process_stereo_output(exception.what());
      }
    }

    TOC();

    if (LAP())
    {
      const auto [sr, bs] = config.value_or(Config{});

      juce::ignoreUnused(sr, bs);

      LOG(CHRONOMETRY(sr, bs));
    }
  }

  virtual std::unique_ptr<Effect> createEffect(const int channel, const double samplerate, const int blocksize) = 0;
  virtual void processMidi(const juce::MidiBuffer& midi) {}

protected:

  struct Config
  {
    double samplerate;
    int blocksize;
  };

  Parameters parameters;
  std::array<std::unique_ptr<Effect>, Channels> effects;

  std::mutex mutex;
  std::optional<Config> config;
  std::optional<bool> bypass;

private:

  JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR(AudioEffectProcessor)

};

END_JUCE_JINGLES_NAMESPACE
