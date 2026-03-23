cmake_minimum_required(VERSION 4.0)

include(GNUInstallDirs)
include(ExternalProject)
include(../common/meson.cmake)

set_property(DIRECTORY PROPERTY EP_UPDATE_DISCONNECTED true)
set(MPV_SOURCE_DIR ${CMAKE_SOURCE_DIR}/libs/mpv)
set(MPV_LIB mpv)

set(MPV_LIB_PATHS "")
foreach(lib IN LISTS MPV_LIB)
    # todo: see if ${CMAKE_SHARED_LIBRARY_SUFFIX} needs to be changed
    list(APPEND MPV_LIB_PATHS ${CMAKE_SOURCE_DIR}/install/lib/lib${lib}${CMAKE_SHARED_LIBRARY_SUFFIX})
endforeach()

ExternalProject_Add(mpv
    SOURCE_DIR ${MPV_SOURCE_DIR}
    BINARY_DIR ${CMAKE_BINARY_DIR}/mpv-build
    CONFIGURE_COMMAND ${MESON_EXECUTABLE} setup ${CMAKE_BINARY_DIR}/mpv-build ${MPV_SOURCE_DIR} --prefix=/
    BUILD_COMMAND ${MESON_EXECUTABLE} compile -C ${CMAKE_BINARY_DIR}/mpv-build
    INSTALL_COMMAND ${MESON_EXECUTABLE} install -C ${CMAKE_BINARY_DIR}/mpv-build --destdir=${CMAKE_SOURCE_DIR}/install
    TEST_COMMAND ""
    BUILD_BYPRODUCTS ${MPV_LIB_PATHS}
    USES_TERMINAL_CONFIGURE true
    USES_TERMINAL_BUILD true
    USES_TERMINAL_INSTALL true
)


add_library(mpv::mpv
    INTERFACE IMPORTED GLOBAL
)

file(MAKE_DIRECTORY ${CMAKE_SOURCE_DIR}/install/include)
target_include_directories(mpv::mpv INTERFACE
    ${CMAKE_SOURCE_DIR}/install/include
)

target_link_libraries(mpv::mpv INTERFACE
    ${MPV_LIB_PATHS}
)

add_dependencies(mpv::mpv mpv)

