cmake_minimum_required(VERSION 4.0)

include(GNUInstallDirs)
include(ExternalProject)
include(../common/autotools.cmake)

set_property(DIRECTORY PROPERTY EP_UPDATE_DISCONNECTED true)
set(FFMPEG_SOURCE_DIR ${CMAKE_SOURCE_DIR}/libs/ffmpeg)
set(FFMPEG_CONFIGURE_FLAGS --prefix=${CMAKE_SOURCE_DIR}/install)

if(BUILD_SHARED_LIBS)
    set(FFMPEG_LIB_SUFFIX ${CMAKE_SHARED_LIBRARY_SUFFIX})
    list(APPEND FFMPEG_CONFIGURE_FLAGS --enable-shared)
else()
    set(FFMPEG_LIB_SUFFIX ${CMAKE_STATIC_LIBRARY_SUFFIX})
    list(APPEND FFMPEG_CONFIGURE_FLAGS --enable-static)
endif()

set(FFMPEG_LIB
    avcodec
    avdevice
    avfilter
    avformat
    avutil
    swscale
    swresample
)

set(FFMPEG_LIB_PATHS "")
foreach(lib IN LISTS FFMPEG_LIB)
    list(APPEND FFMPEG_LIB_PATHS ${CMAKE_SOURCE_DIR}/install/lib/lib${lib}${FFMPEG_LIB_SUFFIX})
endforeach()

ExternalProject_Add(ffmpeg
    SOURCE_DIR ${FFMPEG_SOURCE_DIR}
    CONFIGURE_HANDLED_BY_BUILD true
    CONFIGURE_COMMAND ${FFMPEG_SOURCE_DIR}/configure ${FFMPEG_CONFIGURE_FLAGS}
    BUILD_COMMAND ${MAKE_EXECUTABLE} -j
    INSTALL_COMMAND ${MAKE_EXECUTABLE} install
    TEST_COMMAND ""
    BUILD_BYPRODUCTS ${FFMPEG_LIB_PATHS}
    USES_TERMINAL_CONFIGURE true
    USES_TERMINAL_BUILD true
    USES_TERMINAL_INSTALL true
)

# BUILD_COMMAND ${BEAR_EXECUTABLE} --append --output ${CMAKE_BINARY_DIR}/cmake/ffmpeg/compile_commands.json -- ${MAKE_EXECUTABLE} -j

add_library(ffmpeg::ffmpeg
    INTERFACE IMPORTED GLOBAL
)

file(MAKE_DIRECTORY ${CMAKE_SOURCE_DIR}/install/include)
target_include_directories(ffmpeg::ffmpeg INTERFACE
    ${CMAKE_SOURCE_DIR}/install/include
)

target_link_libraries(ffmpeg::ffmpeg INTERFACE
    ${FFMPEG_LIB_PATHS}
)

add_dependencies(ffmpeg::ffmpeg ffmpeg)
