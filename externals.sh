#!/bin/env bash

function build_glad() {
	glad_source="$(pwd)/subprojects/glad"
	glad_builddir="$(pwd)/build/subprojects/glad"

	cmake -DCMAKE_INSTALL_PREFIX="${INSTALL_DIRECTORY}" -B "${glad_builddir}" -S "${glad_source}" &&
		cmake --build "${glad_builddir}" &&
		cmake --install "${glad_builddir}" --prefix "${INSTALL_DIRECTORY}"
}

function build_libplacebo() {
	libplacebo_source="$(pwd)/subprojects/libplacebo"
	libplacebo_builddir="$(pwd)/build/subprojects/libplacebo"

	meson setup \
		-Dgl-proc-addr=disabled \
		-Dvk-proc-addr=disabled \
		"${libplacebo_builddir}" "${libplacebo_source}" --prefix="${INSTALL_DIRECTORY}" &&
		meson compile -C "${libplacebo_builddir}" &&
		meson install -C "${libplacebo_builddir}"
}

function build_glfw() {
	glfw_source="$(pwd)/subprojects/glfw"
	glfw_builddir="$(pwd)/build/subprojects/glfw"

	cmake -DCMAKE_INSTALL_PREFIX="${INSTALL_DIRECTORY}" -B "${glfw_builddir}" -S "${glfw_source}" &&
		cmake --build "${glfw_builddir}" &&
		cmake --install "${glfw_builddir}" --prefix "${INSTALL_DIRECTORY}"
}

function build_stb() {
	glfw_source="$(pwd)/cmake/stb"
	glfw_builddir="$(pwd)/build/subprojects/stb"

	cmake -DCMAKE_INSTALL_PREFIX="${INSTALL_DIRECTORY}" -B "${glfw_builddir}" -S "${glfw_source}" &&
		cmake --build "${glfw_builddir}" &&
		cmake --install "${glfw_builddir}" --prefix "${INSTALL_DIRECTORY}"
}

function build_cglm() {
	cglm_source="$(pwd)/subprojects/cglm"
	cglm_builddir="$(pwd)/build/subprojects/cglm"

	cmake -DCMAKE_INSTALL_PREFIX="${INSTALL_DIRECTORY}" -B "${cglm_builddir}" -S "${cglm_source}" &&
		cmake --build "${cglm_builddir}" &&
		cmake --install "${cglm_builddir}" --prefix "${INSTALL_DIRECTORY}"
}

function build_cgltf() {
	cgltf_source="$(pwd)/cmake/cgltf"
	cgltf_builddir="$(pwd)/build/subprojects/cgltf"

	cmake -DCMAKE_INSTALL_PREFIX="${INSTALL_DIRECTORY}" -B "${cgltf_builddir}" -S "${cgltf_source}" &&
		cmake --build "${cgltf_builddir}" &&
		cmake --install "${cgltf_builddir}" --prefix "${INSTALL_DIRECTORY}"
}

function build_miniaudio() {
	miniaudio_source="$(pwd)/subprojects/miniaudio"
	miniaudio_builddir="$(pwd)/build/subprojects/miniaudio"

	cmake -DCMAKE_INSTALL_PREFIX="${INSTALL_DIRECTORY}" -B "${miniaudio_builddir}" -S "${miniaudio_source}" &&
		cmake --build "${miniaudio_builddir}" &&
		cmake --install "${miniaudio_builddir}" --prefix "${INSTALL_DIRECTORY}"
}

function build_freetype() {
	freetype_source="$(pwd)/subprojects/freetype"
	freetype_builddir="$(pwd)/build/subprojects/freetype"

	cmake -DCMAKE_INSTALL_PREFIX="${INSTALL_DIRECTORY}" -B "${freetype_builddir}" -S "${freetype_source}" &&
		cmake --build "${freetype_builddir}" &&
		cmake --install "${freetype_builddir}" --prefix "${INSTALL_DIRECTORY}"
}

function build_ffmpeg() {
	ffmpeg_source="$(pwd)/subprojects/ffmpeg"
	ffmpeg_builddir="$(pwd)/build/subprojects/ffmpeg"
	mkdir -p "${ffmpeg_builddir}"

	(cd "${ffmpeg_builddir}" &&
		"${ffmpeg_source}/configure" \
			--prefix="${INSTALL_DIRECTORY}" \
			--arch=x86_64 \
			--cc=clang \
			--cxx=clang++ \
			--enable-gpl \
			--enable-version3 \
			--enable-shared \
			--logfile="${ffmpeg_builddir}/build.log" &&
		make -C "${ffmpeg_builddir}" -j &&
		make -C "${ffmpeg_builddir}" install)
}

function build_libass() {
	libass_source="$(pwd)/subprojects/libass"
	libass_builddir="$(pwd)/build/subprojects/libass"

	meson setup "${libass_builddir}" "${libass_source}" --prefix="${INSTALL_DIRECTORY}" &&
		meson compile -C "${libass_builddir}" &&
		meson install -C "${libass_builddir}"
}

function build_mpv() {
	mpv_source="$(pwd)/subprojects/mpv"
	mpv_builddir="$(pwd)/build/subprojects/mpv"

	meson setup "${mpv_builddir}" "${mpv_source}" --prefix="${INSTALL_DIRECTORY}" &&
		meson compile -C "${mpv_builddir}" &&
		meson install -C "${mpv_builddir}"
}

git submodule update --init --recursive &&
	setup_environment &&
	build_glad &&
	build_glfw &&
	build_stb &&
	build_cglm &&
	build_cgltf &&
	build_freetype &&
	build_miniaudio &&
	build_ffmpeg &&
	build_libplacebo &&
	build_libass &&
	build_mpv
