#include <Robotone/Parameters.h>

Parameters::Parameters(juce::AudioProcessor& process) :
  juce::jingles::XmlParameters(process)
{
  add("bypass", new juce::AudioParameterBool(
    { "bypass", schema }, "Bypass", false,
    juce::AudioParameterBoolAttributes()));
}

bool Parameters::bypass() const
{
  return get<bool>("bypass");
}
