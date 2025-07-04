# https://github.com/jurihock/sdft

CPMAddPackage(
  NAME sdft
  VERSION osdft
  GIT_TAG osdft
  GITHUB_REPOSITORY jurihock/sdft
  DOWNLOAD_ONLY YES)

if(sdft_ADDED)

  add_library(sdft INTERFACE)

  target_include_directories(sdft
    INTERFACE "${sdft_SOURCE_DIR}/cpp/src")

endif()
