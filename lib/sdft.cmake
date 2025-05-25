# https://github.com/jurihock/sdft

CPMAddPackage(
  NAME sdft
  VERSION 1.4
  GIT_TAG f5f30dabe3ef5caea367e03a488cb57af72b5fc6
  GITHUB_REPOSITORY jurihock/sdft
  DOWNLOAD_ONLY YES)

if(sdft_ADDED)

  add_library(sdft INTERFACE)

  target_include_directories(sdft
    INTERFACE "${sdft_SOURCE_DIR}/src/cpp")

endif()
