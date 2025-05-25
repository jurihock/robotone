#include <Robotone/Parameters.h>

Parameters::Parameters(juce::AudioProcessor& process) :
  XmlParameters(process, 1)
{
  add("bypass", new juce::AudioParameterBool(
    { "bypass", schema }, "Bypass", false,
    juce::AudioParameterBoolAttributes()));
}

void Parameters::onbypass(std::function<void()> callback)
{
  call("bypass", std::move(callback));
}

bool Parameters::bypass() const
{
  return get<bool>("bypass");
}
