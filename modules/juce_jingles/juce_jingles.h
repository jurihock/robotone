/*******************************************************************************

  BEGIN_JUCE_MODULE_DECLARATION

  ID:                 juce_jingles
  vendor:             jurihock
  version:            1.0
  name:               JUCE JINGLES
  description:        TODO
  website:            https://github.com/jurihock/juce_jingles
  license:            GPL
  minimumCppStandard: 20
  dependencies:       juce_core

  END_JUCE_MODULE_DECLARATION

*******************************************************************************/

#pragma once

#define JUCE_JINGLES_NAMESPACE       juce::jingles
#define BEGIN_JUCE_JINGLES_NAMESPACE namespace juce { namespace jingles {
#define END_JUCE_JINGLES_NAMESPACE   }}

#if ! DONT_SET_USING_JUCE_NAMESPACE
using namespace JUCE_JINGLES_NAMESPACE;
#endif

#include <juce_core/juce_core.h>

#include <juce_jingles/params/GenericParameterContainer.h>
#include <juce_jingles/params/GenericParameterListener.h>
#include <juce_jingles/params/GenericParameterSubscription.h>
#include <juce_jingles/params/XmlParameters.h>

#include <juce_jingles/utils/Chronometer.h>
#include <juce_jingles/utils/Logger.h>
#include <juce_jingles/utils/Timer.h>
