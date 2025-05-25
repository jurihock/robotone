#pragma once

#include <JuceHeader.h>

#include <Robotone/Utils/Parameters/XmlParameters.h>

class Parameters final : public XmlParameters
{

public:

  Parameters(juce::AudioProcessor& process);

  void onbypass(std::function<void()> callback);

  bool bypass() const;

private:

  JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR(Parameters)

};
