#pragma once

#include <span>

BEGIN_JUCE_JINGLES_NAMESPACE

class AudioEffect
{

public:

  AudioEffect() = default;
  virtual ~AudioEffect() = default;

  virtual bool accepts(const size_t blocksize) const { return true; }
  virtual int latency() const { return 0; }

  virtual void dry(const std::span<const float> input, const std::span<float> output) = 0;
  virtual void wet(const std::span<const float> input, const std::span<float> output) = 0;

private:

  JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR(AudioEffect)

};

END_JUCE_JINGLES_NAMESPACE
