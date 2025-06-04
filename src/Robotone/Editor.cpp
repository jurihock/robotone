#include <Robotone/Editor.h>

Editor::Editor(juce::AudioProcessor& process) :
  juce::GenericAudioProcessorEditor(process)
{
  const int w = 640, h = 480;

  setSize(w, h);
}
