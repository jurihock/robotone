# https://github.com/libsndfile/libsamplerate

CPMAddPackage(
  NAME samplerate
  VERSION 2025-04-07
  GIT_TAG 15c392d47e71b9395a759544b3818a1235fe1a1d
  GITHUB_REPOSITORY libsndfile/libsamplerate
  DOWNLOAD_ONLY YES)

if(samplerate_ADDED)

  set(LIBSAMPLERATE_EXAMPLES OFF CACHE INTERNAL "" FORCE)
  set(LIBSAMPLERATE_INSTALL  OFF CACHE INTERNAL "" FORCE)
  set(BUILD_TESTING          OFF CACHE INTERNAL "" FORCE)

  add_subdirectory(${samplerate_SOURCE_DIR})

endif()
