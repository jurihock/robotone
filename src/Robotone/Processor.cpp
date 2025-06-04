#include <Robotone/Processor.h>

Processor::Processor()
{
}

std::unique_ptr<Effect> Processor::createEffect(const int channel, const double samplerate, const int blocksize)
{
  return std::make_unique<Effect>(samplerate);
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
