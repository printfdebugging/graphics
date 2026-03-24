cmake_minimum_required(VERSION 4.0)
include(GNUInstallDirs)

project(stb
    VERSION         "1.0"
    DESCRIPTION     "stb single-file public domain libraries for C/C++"
    HOMEPAGE_URL    "https://github.com/nothings/stb"
    LANGUAGES       "C"
)

add_library(stb_image stb_image.c)
target_include_directories(stb_image PUBLIC ../../subprojects/stb)

add_library(stb_include stb_include.c)
target_include_directories(stb_include PUBLIC ../../subprojects/stb)
target_compile_options(stb_include PRIVATE -Wno-unused-result)

add_library(stb INTERFACE)
target_link_libraries(stb INTERFACE stb_image stb_include)

install(
    DIRECTORY ../../subprojects/stb
    DESTINATION ${CMAKE_INSTALL_INCLUDEDIR}
)

# todo: create a stb.pc

install(
    TARGETS stb_image stb_include
    EXPORT stbTargets
    LIBRARY DESTINATION ${CMAKE_INSTALL_LIBDIR}
    ARCHIVE DESTINATION ${CMAKE_INSTALL_LIBDIR}
    RUNTIME DESTINATION ${CMAKE_INSTALL_BINDIR}
)
