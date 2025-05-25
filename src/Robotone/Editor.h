#pragma once

#include <JuceHeader.h>

class Editor final : public juce::GenericAudioProcessorEditor
{

public:

  Editor(juce::AudioProcessor& process);

private:

  JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR(Editor)

};
