cmake_minimum_required(VERSION 4.0)

project(game
	VERSION "0.0.1.1"
	DESCRIPTION "Game"
	HOMEPAGE_URL "https://github.com/printfdebugging/graphics"
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

# TODO: comment out this cmake file nicely & also disable the thing which
# rebuilds/configures the project when i make changes to this file


set(CMAKE_C_STANDARD 23)
add_compile_options(-std=gnu23)
set(CMAKE_EXPORT_COMPILE_COMMANDS ON)
set(CMAKE_BUILD_TYPE Debug)

# todo: understand what are these for
set(CMAKE_INSTALL_RPATH "$ORIGIN/../lib")
set(CMAKE_BUILD_WITH_INSTALL_RPATH OFF)
set(CMAKE_INSTALL_RPATH_USE_LINK_PATH ON)

# globals
set(GAME_ASSETS_DIR "${PROJECT_NAME}/assets/")
set(ENGINE_ASSETS_DIR "${ENGINE_PROJECT_NAME}/assets/")

# source
add_executable(${PROJECT_NAME}
	source/main.c
	source/game.c
)

target_compile_definitions(${PROJECT_NAME} PRIVATE
	GLFW_INCLUDE_NONE
	$<IF:$<CONFIG:Debug>,-DDEBUG_BUILD,-DRELEASE_BUILD>
	ENGINE_ASSETS_DIR="$<BUILD_INTERFACE:${CMAKE_SOURCE_DIR}/engine/assets/>$<INSTALL_INTERFACE:${CMAKE_INSTALL_FULL_DATAROOTDIR}/${ENGINE_ASSETS_DIR}>"
	ASSETS_DIR="$<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}/assets/>$<INSTALL_INTERFACE:${CMAKE_INSTALL_FULL_DATAROOTDIR}/${GAME_ASSETS_DIR}>"
)

target_compile_options(${PROJECT_NAME} PRIVATE
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

target_include_directories(${PROJECT_NAME}
	PUBLIC include
)

target_link_libraries(${PROJECT_NAME} PRIVATE
	engine
)

# target_link_options(${APPLICATION} PRIVATE
#         -fsanitize=undefined
#         -fsanitize=address
# )

install(
	TARGETS ${PROJECT_NAME}
	EXPORT  ${PROJECT_NAME}
	LIBRARY DESTINATION ${CMAKE_INSTALL_LIBDIR}
	ARCHIVE DESTINATION ${CMAKE_INSTALL_LIBDIR}
	RUNTIME DESTINATION ${CMAKE_INSTALL_BINDIR}
)

install(
	DIRECTORY "${CMAKE_CURRENT_LIST_DIR}/assets"
	DESTINATION "${CMAKE_INSTALL_DATAROOTDIR}/${PROJECT_NAME}"
)

