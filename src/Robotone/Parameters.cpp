#include <Robotone/Parameters.h>

Parameters::Parameters(juce::AudioProcessor& process) :
  juce::jingles::XmlParameters(process)
{
  add("bypass", new juce::AudioParameterBool(
    { "bypass", schema }, "Bypass", false,
    juce::AudioParameterBoolAttributes()));

  add("decimate", new juce::AudioParameterBool(
    { "decimate", schema }, "Downsample", true,
    juce::AudioParameterBoolAttributes()));

  add("millis", new juce::AudioParameterInt(
    { "millis", schema }, "Window size", 1, 100, 10,
    juce::AudioParameterIntAttributes().withLabel("ms")));

  add("octave", new juce::AudioParameterInt(
    { "octave", schema }, "Octave shift", 0, 10, 5,
    juce::AudioParameterIntAttributes()));

  add("gestalt", new juce::AudioParameterInt(
    { "gestalt", schema }, "Robot vs. Human", -1, 1, 1,
    juce::AudioParameterIntAttributes()));
}
