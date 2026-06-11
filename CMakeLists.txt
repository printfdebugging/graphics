cmake_minimum_required(VERSION 4.0)

project(graphics
	VERSION "0.0.1.1"
	DESCRIPTION "Cross Platform LearnOpenGL Implementaion"
	HOMEPAGE_URL "https://codeberg.org/printfdebugging/learnopengl"
	LANGUAGES "C"
)

include(GNUInstallDirs)

#[[
	CMAKE_PREFIX_PATH points to the install directory of the project.
	it looks for `libfoo.pc` files in ${CMAKE_PREFIX_PATH}/lib/pkgconfig
	directory.
]]
set(CMAKE_PREFIX_PATH "${CMAKE_SOURCE_DIR}/install")
set(CMAKE_COLOR_DIAGNOSTICS ON)

add_subdirectory(cmake/stb)
add_subdirectory(cmake/cgltf)
add_subdirectory(external/cglm SYSTEM)
add_subdirectory(external/glfw)
add_subdirectory(external/glad)
add_subdirectory(external/freetype)
add_subdirectory(external/harfbuzz)
add_subdirectory(external/miniaudio)

add_subdirectory(engine)
add_subdirectory(projects/game)
