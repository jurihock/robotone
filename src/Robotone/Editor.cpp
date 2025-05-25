#include <Robotone/Editor.h>

Editor::Editor(juce::AudioProcessor& process) :
  GenericAudioProcessorEditor(process)
{
  const int w = int(850 / 1.618), h = 850;

  setSize(w, h);
}
