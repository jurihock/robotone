# https://github.com/jurihock/qdft

CPMAddPackage(
  NAME qdft
  VERSION main
  GIT_TAG main
  GITHUB_REPOSITORY jurihock/qdft
  DOWNLOAD_ONLY YES)

if(qdft_ADDED)

  add_library(qdft INTERFACE)

  target_include_directories(qdft
    INTERFACE "${qdft_SOURCE_DIR}/cpp/src")

endif()
