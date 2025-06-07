#include <Robotone/Processor.h>

Processor::Processor()
{
  parameters.notify("milliseconds", [&](auto value)
  {
    std::lock_guard lock(mutex);

    for (size_t i = 0; i < effects.size(); ++i)
    {
      if (effects[i])
      {
        effects[i]->milliseconds(std::get<int>(value));
        effects[i]->reset();
      }
    }
  });

  parameters.notify("decimation", [&](auto value)
  {
    std::lock_guard lock(mutex);

    for (size_t i = 0; i < effects.size(); ++i)
    {
      if (effects[i])
      {
        effects[i]->decimation(std::get<int>(value));
        effects[i]->reset();
      }
    }
  });
}

std::unique_ptr<Effect> Processor::createEffect(const size_t channel, const double samplerate, const size_t blocksize)
{
  auto effect = std::make_unique<Effect>(samplerate);

  effect->milliseconds(parameters.get<int>("milliseconds"));
  effect->decimation(parameters.get<int>("decimation"));
  effect->reset();

  return effect;
}

bool Processor::acceptsMidi() const
{
  return true;
}

void Processor::processMidi(const juce::MidiBuffer& midi)
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
}
