project(Robocode)

add_library(${PROJECT_NAME})

target_include_directories(${PROJECT_NAME}
  PUBLIC
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
  PUBLIC
    samplerate
    sdft)

target_compile_features(${PROJECT_NAME}
  PUBLIC
    cxx_std_20)

if(FASTMATH)

  message(STATUS "Enabling fast math")

  if(MSVC)
    target_compile_options(${PROJECT_NAME}
      PRIVATE
        /fp:fast)
  else()
    target_compile_options(${PROJECT_NAME}
      PRIVATE
        -ffast-math)
  endif()

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
