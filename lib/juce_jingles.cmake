# https://github.com/jurihock/juce_jingles

CPMAddPackage(
  NAME juce_jingles
  VERSION main
  GIT_TAG main
  GITHUB_REPOSITORY jurihock/juce_jingles
  DOWNLOAD_ONLY YES
  SOURCE_DIR "${CPM_SOURCE_CACHE}/juce_jingles")

if(juce_jingles_ADDED)
  juce_add_module("${juce_jingles_SOURCE_DIR}")
endif()
