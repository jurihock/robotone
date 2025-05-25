#include <Robotone/Plugin.h>

#include <Robotone/Processor.h>

juce::AudioProcessor* JUCE_CALLTYPE createPluginFilter()
{
  return new Processor();
}
