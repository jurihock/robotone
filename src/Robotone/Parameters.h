#pragma once

#include <JuceHeader.h>

class Parameters final : public juce::jingles::XmlParameters
{

public:

  Parameters(juce::AudioProcessor& process);

private:

  JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR(Parameters)

};
