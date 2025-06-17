#include <Robotone/Processor.h>

Processor::Processor()
{
  parameters.notify("decimate", [&](auto value)
  {
    std::lock_guard lock(mutex);

    for (size_t i = 0; i < effects.size(); ++i)
    {
      if (effects[i])
      {
        effects[i]->decimate(std::get<bool>(value));
        effects[i]->reset();
      }
    }
  });

  parameters.notify("millis", [&](auto value)
  {
    std::lock_guard lock(mutex);

    for (size_t i = 0; i < effects.size(); ++i)
    {
      if (effects[i])
      {
        effects[i]->millis(std::get<int>(value));
        effects[i]->reset();
      }
    }
  });

  parameters.notify("octave", [&](auto value)
  {
    std::lock_guard lock(mutex);

    for (size_t i = 0; i < effects.size(); ++i)
    {
      if (effects[i])
      {
        effects[i]->octave(std::get<int>(value));
        effects[i]->reset();
      }
    }
  });

  parameters.notify("gestalt", [&](auto value)
  {
    std::lock_guard lock(mutex);

    for (size_t i = 0; i < effects.size(); ++i)
    {
      if (effects[i])
      {
        effects[i]->gestalt(std::get<int>(value));
      }
    }
  });
}

std::unique_ptr<Effect> Processor::createEffect(const size_t channel, const double samplerate, const size_t blocksize)
{
  auto effect = std::make_unique<Effect>(samplerate, blocksize);

  effect->decimate(parameters.get<bool>("decimate"));
  effect->millis(parameters.get<int>("millis"));
  effect->octave(parameters.get<int>("octave"));
  effect->gestalt(parameters.get<int>("gestalt"));
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

    const auto name = juce::MidiMessage::getMidiNoteName(note, true, true, 0).toStdString();
    const auto freq = juce::MidiMessage::getMidiNoteInHertz(note);

    juce::ignoreUnused(name);
    juce::ignoreUnused(freq);

    LOG("MIDI %s #%i %ihz %i%%", name.c_str(), note, int(freq), int(velo * 100));

    for (size_t i = 0; i < effects.size(); ++i)
    {
      effects[i]->update(note, velo);
    }
  }
}
