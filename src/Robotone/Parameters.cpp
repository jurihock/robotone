#include <Robotone/Parameters.h>

Parameters::Parameters(juce::AudioProcessor& process) :
  juce::jingles::XmlParameters(process)
{
  add("bypass", new juce::AudioParameterBool(
    { "bypass", schema }, "Bypass", false,
    juce::AudioParameterBoolAttributes()));

  add("milliseconds", new juce::AudioParameterInt(
    { "milliseconds", schema }, "Window size", 5, 50, 10,
    juce::AudioParameterIntAttributes().withLabel("ms")));

  add("decimation", new juce::AudioParameterInt(
    { "decimation", schema }, "Recimation ratio", 0, 4, 0,
    juce::AudioParameterIntAttributes().withLabel("")));
}
