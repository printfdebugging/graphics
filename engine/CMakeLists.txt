cmake_minimum_required(VERSION 4.0)

project(engine
	VERSION        "0.0.1.1"
	DESCRIPTION    "Graphics Engine Implementaion"
	HOMEPAGE_URL   "https://github.com/printfdebugging/graphics"
	LANGUAGES      "C"
)

include(GNUInstallDirs)

set(CMAKE_C_STANDARD 23)
add_compile_options(-std=gnu2x)
set(CMAKE_EXPORT_COMPILE_COMMANDS ON)
set(CMAKE_BUILD_TYPE Debug)
set(CMAKE_COLOR_DIAGNOSTICS ON)
set(CMAKE_INSTALL_RPATH "$ORIGIN/../lib") # what is this for
set(CMAKE_BUILD_WITH_INSTALL_RPATH OFF) # and this
set(CMAKE_INSTALL_RPATH_USE_LINK_PATH ON) # and this

set(ENGINE_LIB_NAME ${PROJECT_NAME})
set(PROJECT_ASSETS_DIR "${PROJECT_NAME}/assets/")
set(ENGINE_PROJECT_NAME ${PROJECT_NAME} PARENT_SCOPE)

# source
add_library(${PROJECT_NAME}
	source/core/string.c
	source/core/logger.c
	source/core/memory.c
	source/core/context.c

	source/model/primitive.c
	source/model/mesh.c
	source/model/node.c
	source/model/model.c
	source/model/renderer.c

	source/font/font.c
	source/font/atlas.c

	source/shader.c
	source/texture.c
	source/window.c
	source/camera.c
)

target_compile_definitions(${PROJECT_NAME} PRIVATE
	GLFW_INCLUDE_NONE
	$<IF:$<CONFIG:Debug>,-DDEBUG_BUILD,-DRELEASE_BUILD>
	ENGINE_ASSETS_DIR="$<BUILD_INTERFACE:${CMAKE_SOURCE_DIR}/engine/assets/>$<INSTALL_INTERFACE:${CMAKE_INSTALL_FULL_DATAROOTDIR}/${PROJECT_ASSETS_DIR}>"
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
	-fsanitize=undefined
	-fsanitize=address
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

target_link_libraries(${PROJECT_NAME} PUBLIC
	stb
	cgltf
	PkgConfig::GLAD
	PkgConfig::GLFW
	PkgConfig::CGLM
	PkgConfig::HARFBUZZ
	PkgConfig::HARFBUZZ_GPU
	pthread
	m
	$<$<BOOL:${WIN32}>:dwmapi>
	$<$<BOOL:${UNIX}>:vulkan>
	$<$<BOOL:${WIN32}>:vulkan-1>
)

target_link_options(${PROJECT_NAME} PUBLIC
	-fsanitize=undefined
	-fsanitize=address
)

install(
	TARGETS ${PROJECT_NAME}
	EXPORT  ${PROJECT_NAME}
	LIBRARY DESTINATION ${CMAKE_INSTALL_LIBDIR}
	ARCHIVE DESTINATION ${CMAKE_INSTALL_LIBDIR}
	RUNTIME DESTINATION ${CMAKE_INSTALL_BINDIR}
)

install(
	DIRECTORY include/${PROJECT_NAME}
	DESTINATION ${CMAKE_INSTALL_INCLUDEDIR}
)

install(
	DIRECTORY "${CMAKE_CURRENT_LIST_DIR}/assets"
	DESTINATION "${CMAKE_INSTALL_DATAROOTDIR}/${PROJECT_NAME}"
)

configure_file(
	${CMAKE_CURRENT_SOURCE_DIR}/engine.pc.in
	${CMAKE_CURRENT_BINARY_DIR}/engine.pc @ONLY
)

install(
	FILES ${CMAKE_CURRENT_BINARY_DIR}/engine.pc
	DESTINATION ${CMAKE_INSTALL_LIBDIR}/pkgconfig
)
