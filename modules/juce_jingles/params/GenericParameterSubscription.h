#pragma once

#include <juce_jingles/params/GenericParameterListener.h>

BEGIN_JUCE_JINGLES_NAMESPACE

class GenericParameterSubscription final
{

public:

  GenericParameterSubscription(juce::AudioProcessorParameter* parameter,
                               std::function<void()> callback) :
    GenericParameterSubscription(
      parameter,
      std::make_unique<GenericParameterListener>(std::move(callback)))
  {
  }

  GenericParameterSubscription(juce::AudioProcessorParameter* parameter,
                               std::unique_ptr<juce::AudioProcessorParameter::Listener> listener) :
    parameter(parameter),
    listener(std::move(listener))
  {
    subscribe();
  }

  ~GenericParameterSubscription()
  {
    unsubscribe();
  }

private:

  juce::AudioProcessorParameter* parameter;
  std::unique_ptr<juce::AudioProcessorParameter::Listener> listener;

  void subscribe()
  {
    if (parameter && listener)
    {
      parameter->addListener(listener.get());
    }
  }

  void unsubscribe()
  {
    if (parameter && listener)
    {
      parameter->removeListener(listener.get());
    }

    parameter = nullptr;
    listener = nullptr;
  }

};

END_JUCE_JINGLES_NAMESPACE
