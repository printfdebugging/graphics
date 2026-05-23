cmake_minimum_required(VERSION 4.0)

if (NOT EMSCRIPTEN)
   install(
      DIRECTORY "${CMAKE_SOURCE_DIR}/assets"
      DESTINATION "${CMAKE_INSTALL_DATAROOTDIR}/${PROJECT_NAME}"
   )
endif()
