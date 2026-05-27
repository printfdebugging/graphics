cmake_minimum_required(VERSION 4.0)

project(LearnOpenGL
        VERSION        "0.0.1.1"
        DESCRIPTION    "Cross Platform LearnOpenGL Implementaion"
        HOMEPAGE_URL   "https://codeberg.org/printfdebugging/learnopengl"
        LANGUAGES      "C"
)

include(GNUInstallDirs)

#[[
CMAKE_PREFIX_PATH points to the install directory of the project.
it looks for `libfoo.pc` files in ${CMAKE_PREFIX_PATH}/lib/pkgconfig
directory.
]]
set(CMAKE_PREFIX_PATH "${CMAKE_SOURCE_DIR}/install")
set(CMAKE_COLOR_DIAGNOSTICS ON)

# TODO: comment out this cmake file nicely & also disable the thing which
# rebuilds/configures the project when i make changes to this file

add_subdirectory(cmake/stb)
add_subdirectory(cmake/cgltf)
add_subdirectory(subprojects/cglm)
add_subdirectory(subprojects/glfw)
add_subdirectory(subprojects/glad)
add_subdirectory(subprojects/freetype)
add_subdirectory(subprojects/harfbuzz)
add_subdirectory(subprojects/miniaudio)

set(CMAKE_C_STANDARD 23)
add_compile_options(-std=gnu23)
set(CMAKE_C_COMPILER_LAUNCHER "ccache")
set(CMAKE_CXX_COMPILER_LAUNCHER "ccache")
set(CMAKE_EXPORT_COMPILE_COMMANDS ON)
set(CMAKE_BUILD_TYPE Debug)

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
        source/game.c
        source/mesh.c
        source/model.c
        source/renderer.c
        source/shader.c
        source/texture.c
        source/window.c
        source/core/string.c
        source/camera.c
)

target_compile_definitions(${APPLICATION} PRIVATE
        GLFW_INCLUDE_NONE
        $<IF:$<CONFIG:Debug>,-DDEBUG_BUILD,-DRELEASE_BUILD>
        ASSETS_DIR="$<BUILD_INTERFACE:${CMAKE_SOURCE_DIR}/assets/>$<INSTALL_INTERFACE:${CMAKE_INSTALL_FULL_DATAROOTDIR}/${PROJECT_ASSETS_DIR}>"
)

target_compile_options(${APPLICATION} PRIVATE
        -Wall
        -Wextra
        -Wundef
        -Wshadow
        -Wpointer-arith
        -Wcast-align
        -Wstrict-prototypes
        -Wwrite-strings
        -Wcast-qual
        -Wswitch-default
        -Wswitch-enum
        -Wconversion
        -Wunreachable-code
        -Wpedantic
        -fsanitize=undefined
        -fsanitize=address
        # -Wfloat-equal
        # -Werror
)

target_include_directories(${APPLICATION}
        PUBLIC include
)

target_link_libraries(${APPLICATION} PUBLIC
        stb
        cgltf
        glad
        glfw
        freetype
        harfbuzz
        miniaudio
        cglm
        pthread
        m
        $<$<BOOL:${WIN32}>:dwmapi>
        $<$<BOOL:${UNIX}>:vulkan>
        $<$<BOOL:${WIN32}>:vulkan-1>
)

target_link_options(${APPLICATION} PRIVATE
        -fsanitize=undefined
        -fsanitize=address
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
