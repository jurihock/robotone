cmake_minimum_required(VERSION 3.25) # debian bookworm

project(Robotone)

option(FASTATAN "Enable fast atan2"   OFF)
option(FASTMATH "Enable fast math"    OFF)
option(LOGGING  "Enable runtime logs" OFF)
option(CHRONO   "Enable timing logs"  OFF)
option(WARNINGS "Enable all warnings" ON)

include("${CMAKE_CURRENT_LIST_DIR}/lib/cpm.cmake")
include("${CMAKE_CURRENT_LIST_DIR}/lib/juce.cmake")
include("${CMAKE_CURRENT_LIST_DIR}/lib/juce_jingles.cmake")
include("${CMAKE_CURRENT_LIST_DIR}/lib/samplerate.cmake")
include("${CMAKE_CURRENT_LIST_DIR}/lib/sdft.cmake")
include("${CMAKE_CURRENT_LIST_DIR}/lib/pluginval.cmake")

add_subdirectory("${CMAKE_CURRENT_LIST_DIR}/src/Robocode")
add_subdirectory("${CMAKE_CURRENT_LIST_DIR}/src/Robotone")
