#include <Robotone/Processor.h>

Processor::Processor()
{
  parameters->notify("bypass", [&]()
  {
    std::lock_guard lock(mutex);

    bypass = parameters->bypass();
  });
}

bool Processor::acceptsMidi() const
{
  return true;
}

void Processor::releaseResources()
{
  std::lock_guard lock(mutex);

  LOG("Release resources");

  config.reset();
  effects.clear();
}

void Processor::prepareToPlay(double samplerate, int blocksize)
{
  std::lock_guard lock(mutex);

  LOG("Release resources");

  config.reset();
  effects.clear();

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

  bypass = parameters->bypass();

  try
  {
    for (auto channel = 0; channel < 1; ++channel) // TODO
    {
      auto effect = std::make_unique<Effect>(samplerate);

      effects.push_back(std::move(effect));

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

    effects.clear();
  }
}

void Processor::processBlock(juce::AudioBuffer<float>& audio, juce::MidiBuffer& midi)
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

  const auto process_midi_input = [&]()
  {
    for (const juce::MidiMessageMetadata& meta : midi)
    {
      const juce::MidiMessage msg = meta.getMessage();

      const auto on = msg.isNoteOn();
      const auto off = msg.isNoteOff();

      if (!on && !off)
      {
        continue;
      }

      const auto note = msg.getNoteNumber();
      const auto velo = on ? msg.getFloatVelocity() : 0;
      const auto freq = juce::MidiMessage::getMidiNoteInHertz(note);

      juce::ignoreUnused(freq);

      LOG("MIDI %i %f %f", note, freq, velo);

      for (size_t i = 0; i < effects.size(); ++i)
      {
        effects[i]->update(note, velo);
      }
    }
  };

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

      if (bypass.value_or(parameters->bypass()))
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
      process_midi_input();
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
