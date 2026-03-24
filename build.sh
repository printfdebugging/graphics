#!/bin/env bash

install_directory=./install
install_prefix=/

function build_glad() {
	glad_source=./subprojects/glad
	glad_builddir=./build/subprojects/glad

	cmake -DCMAKE_INSTALL_PREFIX=${install_directory} -B ${glad_builddir} -S ${glad_source} &&
		cmake --build ${glad_builddir} &&
		cmake --install ${glad_builddir} --prefix ${install_directory}
}

function build_libplacebo() {
	libplacebo_source=./subprojects/libplacebo
	libplacebo_builddir=./build/subprojects/libplacebo

	meson setup ${libplacebo_builddir} ${libplacebo_source} --prefix=${install_prefix} &&
		meson compile -C ${libplacebo_builddir} &&
		meson install -C ${libplacebo_builddir} --destdir=${install_directory}
}

function build_glfw() {
	glfw_source=./subprojects/glfw
	glfw_builddir=./build/subprojects/glfw

	cmake -DCMAKE_INSTALL_PREFIX=${install_directory} -B "${glfw_builddir}" -S "${glfw_source}" &&
		cmake --build ${glfw_builddir} &&
		cmake --install ${glfw_builddir} --prefix ${install_directory}
}

function build_stb() {
	glfw_source=./cmake/stb
	glfw_builddir=./build/subprojects/stb

	cmake -DCMAKE_INSTALL_PREFIX=${install_directory} -B "${glfw_builddir}" -S "${glfw_source}" &&
		cmake --build ${glfw_builddir} &&
		cmake --install ${glfw_builddir} --prefix ${install_directory}
}

# todo: each of these should have their own pkgconfigs

build_glad &&
	build_glfw &&
	build_stb

# build_ffmpeg

# .PHONY: build
# build:
# 	cmake -B build
# 	cmake --build build -j 32 -v
#
# run:
# 	cmake -B build
# 	cmake --build build -j 32 -v
# 	./build/executable
#
# install: build
# 	cmake --install build --prefix install
#
# clean:
# 	rm -rf build
# 	rm -rf install
