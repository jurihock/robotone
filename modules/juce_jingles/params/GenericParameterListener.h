#pragma once

#include <juce_core/juce_core.h>

BEGIN_JUCE_JINGLES_NAMESPACE

class GenericParameterListener final : public juce::AudioProcessorParameter::Listener
{

public:

  GenericParameterListener(std::function<void()> callback) :
    callback(std::move(callback))
  {
  }

  void parameterValueChanged(int index, float value) override
  {
    callback();
  }

  void parameterGestureChanged(int index, bool gesture) override
  {
  }

private:

  const std::function<void()> callback;

};

END_JUCE_JINGLES_NAMESPACE
