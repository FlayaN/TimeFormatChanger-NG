cmake_minimum_required(VERSION 3.26)

set(PROJECT_AUTHOR "Vermunds")
project(
  TimeFormatChanger
  VERSION 1.2.1
  LANGUAGES CXX)

# ##############################################################################
# # Source build guard
# ##############################################################################

if(PROJECT_SOURCE_DIR STREQUAL PROJECT_BINARY_DIR)
  message(
    FATAL_ERROR
      "In-source builds not allowed. Please make a new directory (called a build directory) and run CMake from there."
  )
endif()

# ##############################################################################
# # Add CommonLibNG plugin
# ##############################################################################

list(APPEND CMAKE_MODULE_PATH "${PROJECT_SOURCE_DIR}/cmake")
include(CLIBNGPlugin)
add_simple_commonlibsse_ng_plugin(AUTHOR "${PROJECT_AUTHOR}" LICENSE
                                  "MIT License")

# ##############################################################################
# # Build options
# ##############################################################################

message("Options:")
option(ZIP_TO_DIST "Zip the mod to 7z file in dist." ON)
message("\tZip to dist: ${ZIP_TO_DIST}")

# ##############################################################################
# # Find dependencies
# ##############################################################################

find_path(CLIB_UTIL_INCLUDE_DIRS "ClibUtil/detail/SimpleIni.h")

# ##############################################################################
# # Post target setup
# ##############################################################################

target_include_directories(${PROJECT_NAME} PRIVATE ${CLIB_UTIL_INCLUDE_DIRS})

# ##############################################################################
# # Generates a zip in /dist
# ##############################################################################

if(ZIP_TO_DIST)
  set(ZIP_DIR "${CMAKE_CURRENT_BINARY_DIR}/zip")
  add_custom_command(
    TARGET ${PROJECT_NAME}
    POST_BUILD
    COMMAND ${CMAKE_COMMAND} -E remove_directory "${ZIP_DIR}"
            ${CMAKE_SOURCE_DIR}/dist
    COMMAND ${CMAKE_COMMAND} -E make_directory "${ZIP_DIR}/SKSE/Plugins"
            ${CMAKE_SOURCE_DIR}/dist)

  message("Copying mod to ${ZIP_DIR}.")
  add_custom_command(
    TARGET ${PROJECT_NAME}
    POST_BUILD
    COMMAND ${CMAKE_COMMAND} -E copy $<TARGET_FILE:${PROJECT_NAME}>
            "${ZIP_DIR}/SKSE/Plugins/"
    COMMAND ${CMAKE_COMMAND} -E copy $<TARGET_PDB_FILE:${PROJECT_NAME}>
            "${ZIP_DIR}/SKSE/Plugins/")

  set(TARGET_ZIP "${PROJECT_NAME}${PROJECT_VERSION}.7z")
  message("Zipping ${ZIP_DIR} to ${CMAKE_SOURCE_DIR}/dist/${TARGET_ZIP}")
  add_custom_command(
    TARGET ${PROJECT_NAME}
    POST_BUILD
    COMMAND ${CMAKE_COMMAND} -E tar cf ${CMAKE_SOURCE_DIR}/dist/${TARGET_ZIP}
            --format=7zip -- .
    WORKING_DIRECTORY ${ZIP_DIR})
endif()
