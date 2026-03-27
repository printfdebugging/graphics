cmake_minimum_required(VERSION 4.0)

project(LearnOpenGL
    VERSION         "0.0.1.1"
    DESCRIPTION     "Cross Platform LearnOpenGL Implementaion"
    HOMEPAGE_URL    "https://codeberg.org/printfdebugging/learnopengl"
    LANGUAGES       "C"
)

include(GNUInstallDirs)
set(ENV{PKG_CONFIG_PATH} ${CMAKE_SOURCE_DIR}/install/lib/pkgconfig:$ENV{PKG_CONFIG_PATH})
find_package(PkgConfig REQUIRED)

pkg_check_modules(STB REQUIRED IMPORTED_TARGET stb)
pkg_check_modules(GLFW REQUIRED IMPORTED_TARGET glfw3)
pkg_check_modules(CGLTF REQUIRED IMPORTED_TARGET cgltf)
pkg_check_modules(FREETYPE REQUIRED IMPORTED_TARGET freetype2)
pkg_check_modules(MINIAUDIO REQUIRED IMPORTED_TARGET miniaudio)
pkg_check_modules(CGLM REQUIRED IMPORTED_TARGET cglm)
pkg_check_modules(GLAD REQUIRED IMPORTED_TARGET glad)
# todo: fix mpv linking problems on linux
pkg_check_modules(MPV REQUIRED IMPORTED_TARGET mpv)
pkg_check_modules(FFMPEG_AVCODEC REQUIRED IMPORTED_TARGET libavcodec)

set(CMAKE_C_STANDARD 23)
set(CMAKE_C_COMPILER_LAUNCHER "ccache")
set(CMAKE_CXX_COMPILER_LAUNCHER "ccache")
set(CMAKE_EXPORT_COMPILE_COMMANDS ON)
set(CMAKE_BUILD_TYPE Debug)
set(CMAKE_THREAD_LIBS_INIT "-lpthread")
set(CMAKE_HAVE_THREADS_LIBRARY 1)
set(CMAKE_USE_WIN32_THREADS_INIT 0)
set(CMAKE_USE_PTHREADS_INIT 1)
set(THREADS_PREFER_PTHREAD_FLAG ON)

if (WIN32)
    set(CPACK_NSIS_MODIFY_PATH ON)
    set(CPACK_GENERATOR "NSIS")
endif()

set(CMAKE_INSTALL_RPATH "$ORIGIN/../lib")
set(CMAKE_BUILD_WITH_INSTALL_RPATH OFF)
set(CMAKE_INSTALL_RPATH_USE_LINK_PATH ON)

# globals
set(APPLICATION "executable")
set(PROJECT_ASSETS_DIR "${PROJECT_NAME}/assets/")

# source
add_executable(${APPLICATION}
    source/logger.c
    source/main.c
    source/mesh.c
    source/renderer.c
    source/shader.c
    source/texture.c
    source/window.c
    source/utils.c
    source/camera.c
)

target_compile_definitions(${APPLICATION} PRIVATE
    GLFW_INCLUDE_NONE
    $<IF:$<CONFIG:Debug>,-DDEBUG_BUILD,-DRELEASE_BUILD>
    ASSETS_DIR="$<BUILD_INTERFACE:${CMAKE_SOURCE_DIR}/assets/>$<INSTALL_INTERFACE:${CMAKE_INSTALL_FULL_DATAROOTDIR}/${PROJECT_ASSETS_DIR}>"
)

target_include_directories(${APPLICATION}
    PUBLIC include
)

target_link_libraries(${APPLICATION} PUBLIC
    PkgConfig::STB
    PkgConfig::GLFW
    PkgConfig::CGLTF
    PkgConfig::FREETYPE
    PkgConfig::MINIAUDIO
    PkgConfig::CGLM
    PkgConfig::GLAD
    PkgConfig::FFMPEG_AVCODEC
    PkgConfig::MPV
    pthread
    m
    $<$<BOOL:${WIN32}>:dwmapi>
    $<$<BOOL:${UNIX}>:vulkan>
    $<$<BOOL:${WIN32}>:vulkan-1>
)

install(
    TARGETS ${APPLICATION}
    EXPORT  ${APPLICATION}
    LIBRARY DESTINATION ${CMAKE_INSTALL_LIBDIR}
    ARCHIVE DESTINATION ${CMAKE_INSTALL_LIBDIR}
    RUNTIME DESTINATION ${CMAKE_INSTALL_BINDIR}
)

install(
    DIRECTORY "${CMAKE_CURRENT_LIST_DIR}/assets"
    DESTINATION "${CMAKE_INSTALL_DATAROOTDIR}/${PROJECT_NAME}"
)
