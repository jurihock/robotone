#include <Robotone/Parameters.h>

Parameters::Parameters(juce::AudioProcessor& process) :
  juce::jingles::XmlParameters(process)
{
  add("bypass", new juce::AudioParameterBool(
    { "bypass", schema }, "Bypass", false,
    juce::AudioParameterBoolAttributes()));

  add("millis", new juce::AudioParameterInt(
    { "millis", schema }, "Window size", 1, 100, 10,
    juce::AudioParameterIntAttributes().withLabel("ms")));

  add("octave", new juce::AudioParameterInt(
    { "octave", schema }, "Octave shift", -3, +3, 0,
    juce::AudioParameterIntAttributes().withLabel("")));
}
