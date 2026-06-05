cmake_minimum_required(VERSION 4.0)

project(Engine
        VERSION        "0.0.1.1"
        DESCRIPTION    "Graphics Engine Implementaion"
        HOMEPAGE_URL   "https://github.com/printfdebugging/graphics"
        LANGUAGES      "C"
)

include(GNUInstallDirs)

set(CMAKE_C_STANDARD 23)
add_compile_options(-std=gnu23)
set(CMAKE_EXPORT_COMPILE_COMMANDS ON)
set(CMAKE_BUILD_TYPE Debug)
set(CMAKE_COLOR_DIAGNOSTICS ON)
set(CMAKE_INSTALL_RPATH "$ORIGIN/../lib") # what is this for
set(CMAKE_BUILD_WITH_INSTALL_RPATH OFF) # and this
set(CMAKE_INSTALL_RPATH_USE_LINK_PATH ON) # and this

# globals
set(LIBRARY "engine")
set(PROJECT_ASSETS_DIR "${PROJECT_NAME}/assets/")
set(ENGINE_PROJECT_NAME ${PROJECT_NAME} PARENT_SCOPE)

# source
add_library(${LIBRARY}
        source/core/string.c
        source/core/logger.c

        source/primitive.c
        source/model.c
        source/renderer.c
        source/shader.c
        source/texture.c
        source/window.c
        source/camera.c
)

target_compile_definitions(${LIBRARY} PRIVATE
        GLFW_INCLUDE_NONE
        $<IF:$<CONFIG:Debug>,-DDEBUG_BUILD,-DRELEASE_BUILD>
)

target_compile_options(${LIBRARY} PRIVATE
        -Wall
        -Wextra
        -Wundef
        -Wshadow
        -Wpointer-arith
        -Wcast-align
        -Wstrict-prototypes
        -Wwrite-strings
        -Wswitch-default
        -Wswitch-enum
        -Wconversion
        -Wunreachable-code
        -Wpedantic
        # -fsanitize=undefined
        # -fsanitize=address
        -DLSAN_OPTIONS=verbosity=1:log_threads=1
        # -Wcast-qual
        # -Wfloat-equal
        # -Werror
        -Wno-override-init
        -Wno-override-init-side-effects
)

target_include_directories(${LIBRARY}
        PUBLIC include
)

target_link_libraries(${LIBRARY} PUBLIC
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

# target_link_options(${APPLICATION} PRIVATE
#         -fsanitize=undefined
#         -fsanitize=address
# )

install(
        TARGETS ${LIBRARY}
        EXPORT  ${LIBRARY}
        LIBRARY DESTINATION ${CMAKE_INSTALL_LIBDIR}
        ARCHIVE DESTINATION ${CMAKE_INSTALL_LIBDIR}
        RUNTIME DESTINATION ${CMAKE_INSTALL_BINDIR}
)

install(
        DIRECTORY "${CMAKE_CURRENT_LIST_DIR}/assets"
        DESTINATION "${CMAKE_INSTALL_DATAROOTDIR}/${PROJECT_NAME}"
)
