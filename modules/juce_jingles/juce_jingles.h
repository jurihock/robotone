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
  dependencies:       juce_core juce_audio_processors

  END_JUCE_MODULE_DECLARATION

*******************************************************************************/

#pragma once

#define JUCE_JINGLES_NAMESPACE       juce::jingles
#define BEGIN_JUCE_JINGLES_NAMESPACE namespace JUCE_JINGLES_NAMESPACE {
#define END_JUCE_JINGLES_NAMESPACE   }

#if ! DONT_SET_USING_JUCE_NAMESPACE
BEGIN_JUCE_JINGLES_NAMESPACE
END_JUCE_JINGLES_NAMESPACE
using namespace JUCE_JINGLES_NAMESPACE;
#endif

BEGIN_JUCE_JINGLES_NAMESPACE
const char* const juce_jingles_project_name();
const char* const juce_jingles_version_string();
END_JUCE_JINGLES_NAMESPACE

#define HELLO_JUCE_JINGLES \
BEGIN_JUCE_JINGLES_NAMESPACE \
const char* const juce_jingles_project_name()   { return ProjectInfo::projectName;   } \
const char* const juce_jingles_version_string() { return ProjectInfo::versionString; } \
END_JUCE_JINGLES_NAMESPACE

#define JUCE_JINGLES_PROJECT JUCE_JINGLES_NAMESPACE::juce_jingles_project_name()
#define JUCE_JINGLES_VERSION JUCE_JINGLES_NAMESPACE::juce_jingles_version_string()

#include <juce_core/juce_core.h>
#include <juce_audio_processors/juce_audio_processors.h>

#include <juce_jingles/audio/AudioEffect.h>
#include <juce_jingles/audio/AudioEffectProcessor.h>

#include <juce_jingles/params/GenericParameterContainer.h>
#include <juce_jingles/params/GenericParameterListener.h>
#include <juce_jingles/params/GenericParameterSubscription.h>
#include <juce_jingles/params/XmlParameters.h>

#include <juce_jingles/utils/Chronometer.h>
#include <juce_jingles/utils/Logger.h>
#include <juce_jingles/utils/Timer.h>
