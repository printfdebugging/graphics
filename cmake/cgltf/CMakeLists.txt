cmake_minimum_required(VERSION 4.0)
include(GNUInstallDirs)

project(cgltf
    VERSION         "1.0"
    DESCRIPTION     "Single-file glTF 2.0 loader and writer written in C99"
    HOMEPAGE_URL    "https://github.com/jkuhlmann/cgltf"
    LANGUAGES       "C"
)

add_library(cgltf
    cgltf.c
    cgltf_write.c
)

target_include_directories(cgltf PUBLIC ../../subprojects/cgltf)

install(
    DIRECTORY ../../subprojects/cgltf
    DESTINATION ${CMAKE_INSTALL_INCLUDEDIR}
)

install(
    TARGETS cgltf
    LIBRARY DESTINATION ${CMAKE_INSTALL_LIBDIR}
    ARCHIVE DESTINATION ${CMAKE_INSTALL_LIBDIR}
    RUNTIME DESTINATION ${CMAKE_INSTALL_BINDIR}
)
