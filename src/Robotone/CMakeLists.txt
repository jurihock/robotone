project(Robotone
  DESCRIPTION "MIDI-controlled polyphonic vocal harmonizer"
  VERSION 1.0)

set(FORMATS Standalone VST3)

if(CMAKE_SYSTEM_NAME STREQUAL "Darwin")
  list(APPEND FORMATS AU)
endif()

if(CMAKE_SYSTEM_NAME STREQUAL "Linux")
  list(APPEND FORMATS LV2)
endif()

juce_add_plugin(${PROJECT_NAME}
  PLUGIN_NAME                   "${PROJECT_NAME} v${PROJECT_VERSION}"
  DESCRIPTION                   "${PROJECT_DESCRIPTION}"
  PRODUCT_NAME                  "${PROJECT_NAME}"
  VERSION                       "${PROJECT_VERSION}"
  COMPANY_NAME                  "jurihock"
  COMPANY_EMAIL                 "juergen.hock@jurihock.de"
  COMPANY_WEBSITE               "https://github.com/jurihock/robotone"
  BUNDLE_ID                     "de.jurihock.robotone"
  PLUGIN_MANUFACTURER_CODE      "Hock"
  PLUGIN_CODE                   "Rbtn"
  IS_SYNTH                      FALSE
  IS_MIDI_EFFECT                FALSE
  NEEDS_MIDI_INPUT              TRUE
  NEEDS_MIDI_OUTPUT             FALSE
  MICROPHONE_PERMISSION_ENABLED TRUE
  COPY_PLUGIN_AFTER_BUILD       FALSE
  FORMATS                       ${FORMATS})

juce_generate_juce_header(${PROJECT_NAME})

target_include_directories(${PROJECT_NAME}
  PRIVATE
    "${CMAKE_CURRENT_LIST_DIR}/..")

file(
  GLOB_RECURSE
    HDR "${CMAKE_CURRENT_LIST_DIR}/*.h"
    CPP "${CMAKE_CURRENT_LIST_DIR}/*.cpp")

target_sources(${PROJECT_NAME}
  PRIVATE
    "${HDR}"
    "${CPP}")

target_link_libraries(${PROJECT_NAME}
  PRIVATE
    juce_jingles
    robocode)

target_link_libraries(${PROJECT_NAME}
  PRIVATE
    juce::juce_audio_utils
  PUBLIC
    juce::juce_recommended_config_flags
    juce::juce_recommended_lto_flags)

target_compile_definitions(${PROJECT_NAME}
  PUBLIC
    DONT_SET_USING_JUCE_NAMESPACE=1
    JUCE_USE_CURL=0
    JUCE_VST3_CAN_REPLACE_VST2=0
    JUCE_WEB_BROWSER=0)

if(LOGGING)

  message(STATUS "Defining ENABLE_JINGLE_LOGGER")

  target_compile_definitions(${PROJECT_NAME}
    PRIVATE
      -DENABLE_JINGLE_LOGGER)

endif()

if(CHRONO)

  message(STATUS "Defining ENABLE_JINGLE_CHRONOMETER")

  target_compile_definitions(${PROJECT_NAME}
    PRIVATE
      -DENABLE_JINGLE_CHRONOMETER)

endif()

if(WARNINGS)

  message(STATUS "Enabling all warnings")

  if(MSVC)
    target_compile_options(${PROJECT_NAME}
      PRIVATE
        /W3 /WX)
  else()
    target_compile_options(${PROJECT_NAME}
      PRIVATE
        -Wall -Werror)
  endif()

endif()

if(CMAKE_SYSTEM_NAME STREQUAL "Darwin")

  # FIX
  # If you are using Link Time Optimisation (LTO), the new linker introduced in Xcode 15 may produce a broken binary.
  # As a workaround, add either '-Wl,-weak_reference_mismatches,weak' or '-Wl,-ld_classic' to your linker flags.
  # Once you've selected a workaround, you can add JUCE_SILENCE_XCODE_15_LINKER_WARNING
  # to your preprocessor definitions to silence this warning.
  # https://forum.juce.com/t/vst-au-builds-fail-after-upgrading-to-xcode-15/57936/43

  message(STATUS "Applying -Wl -ld_classic JUCE_SILENCE_XCODE_15_LINKER_WARNING")

  target_link_options(${PROJECT_NAME}
    PRIVATE
      -Wl,-ld_classic)

  target_compile_definitions(${PROJECT_NAME}
    PRIVATE
      JUCE_SILENCE_XCODE_15_LINKER_WARNING)

endif()
