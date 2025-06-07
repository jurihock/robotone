# https://github.com/jurihock/juce_jingles

CPMAddPackage(
  NAME juce_jingles
  VERSION main
  GIT_TAG main
  GITHUB_REPOSITORY jurihock/juce_jingles
  DOWNLOAD_ONLY YES)

if(juce_jingles_ADDED)
  juce_add_module("${juce_jingles_SOURCE_DIR}/juce_jingles")
endif()
