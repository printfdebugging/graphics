cmake_minimum_required(VERSION 4.0)

project(graphics
	VERSION "0.0.1.1"
	DESCRIPTION "Cross Platform LearnOpenGL Implementaion"
	HOMEPAGE_URL "https://codeberg.org/printfdebugging/learnopengl"
	LANGUAGES "C"
)

find_program(CCACHE_PROGRAM ccache)
if(CCACHE_PROGRAM)
	set(CMAKE_C_COMPILER_LAUNCHER "ccache")
	set(CMAKE_CXX_COMPILER_LAUNCHER "ccache")
endif()

include(GNUInstallDirs)

#[[
	CMAKE_PREFIX_PATH points to the install directory of the project.
	it looks for `libfoo.pc` files in ${CMAKE_PREFIX_PATH}/lib/pkgconfig
	directory.
]]
set(CMAKE_PREFIX_PATH "${CMAKE_SOURCE_DIR}/install")
set(CMAKE_RUNTIME_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/bin)
set(CMAKE_COLOR_DIAGNOSTICS ON)
set(BUILD_SHARED_LIBS ON)

find_package(PkgConfig REQUIRED)
pkg_check_modules(GLFW REQUIRED IMPORTED_TARGET GLOBAL glfw3)
pkg_check_modules(GLAD REQUIRED IMPORTED_TARGET GLOBAL glad)
pkg_check_modules(CGLM REQUIRED IMPORTED_TARGET GLOBAL cglm)
pkg_check_modules(FONTCONFIG REQUIRED IMPORTED_TARGET GLOBAL fontconfig)
pkg_check_modules(HARFBUZZ REQUIRED IMPORTED_TARGET GLOBAL harfbuzz)
pkg_check_modules(HARFBUZZ_GPU REQUIRED IMPORTED_TARGET GLOBAL harfbuzz-gpu)

add_subdirectory(cmake/stb)
add_subdirectory(cmake/cgltf)
add_subdirectory(engine)
add_subdirectory(projects/game)
add_subdirectory(projects/editor)
