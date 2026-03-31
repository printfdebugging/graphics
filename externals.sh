#!/bin/env bash

export INSTALL_DIRECTORY="$(pwd)/install"
export PKG_CONFIG_PATH="$(pwd)/install/lib/pkgconfig"
export BUILD_TYPE_CMAKE="Debug"
export BUILD_TYPE_MESON="debug"

bear_config="$(pwd)/bear.yml"
bear_output="$(pwd)/compile_commands.json"
bear_script="$(pwd)/install/bin/bear"
bear_cmd="${bear_script} --config ${bear_config} --output ${bear_output} --append --"

function build_bear() {
	bear_source="$(pwd)/subprojects/bear"
	bear_prefix="$(pwd)/install"
	(cd "${bear_source}" && cargo build --release)
	PREFIX="${bear_prefix}" "${bear_source}"/scripts/install.sh
}

function build_glad() {
	glad_source="$(pwd)/subprojects/glad"
	glad_builddir="$(pwd)/build/subprojects/glad"

	${bear_cmd} cmake \
		-DCMAKE_BUILD_TYPE="${BUILD_TYPE_CMAKE}" \
		-DCMAKE_INSTALL_PREFIX="${INSTALL_DIRECTORY}" \
		-B "${glad_builddir}" -S "${glad_source}" &&
		${bear_cmd} cmake --build "${glad_builddir}" &&
		cmake --install "${glad_builddir}" --prefix "${INSTALL_DIRECTORY}"
}

function build_libplacebo() {
	libplacebo_source="$(pwd)/subprojects/libplacebo"
	libplacebo_builddir="$(pwd)/build/subprojects/libplacebo"

	meson setup \
		-Dbuildtype="${BUILD_TYPE_MESON}" \
		-Dgl-proc-addr=disabled \
		-Dvk-proc-addr=disabled \
		"${libplacebo_builddir}" "${libplacebo_source}" --prefix="${INSTALL_DIRECTORY}" &&
		${bear_cmd} meson compile -C "${libplacebo_builddir}" &&
		meson install -C "${libplacebo_builddir}"
}

function build_glfw() {
	glfw_source="$(pwd)/subprojects/glfw"
	glfw_builddir="$(pwd)/build/subprojects/glfw"

	${bear_cmd} cmake \
		-DCMAKE_BUILD_TYPE="${BUILD_TYPE_CMAKE}" \
		-DCMAKE_INSTALL_PREFIX="${INSTALL_DIRECTORY}" \
		-B "${glfw_builddir}" -S "${glfw_source}" &&
		${bear_cmd} cmake --build "${glfw_builddir}" &&
		cmake --install "${glfw_builddir}" --prefix "${INSTALL_DIRECTORY}"
}

function build_sdl() {
	sdl_source="$(pwd)/subprojects/sdl"
	sdl_builddir="$(pwd)/build/subprojects/sdl"

	${bear_cmd} cmake \
		-DCMAKE_BUILD_TYPE="${BUILD_TYPE_CMAKE}" \
		-DCMAKE_INSTALL_PREFIX="${INSTALL_DIRECTORY}" \
		-B "${sdl_builddir}" -S "${sdl_source}" &&
		${bear_cmd} cmake --build "${sdl_builddir}" &&
		cmake --install "${sdl_builddir}" --prefix "${INSTALL_DIRECTORY}"
}

function build_stb() {
	glfw_source="$(pwd)/cmake/stb"
	glfw_builddir="$(pwd)/build/subprojects/stb"

	${bear_cmd} cmake \
		-DCMAKE_BUILD_TYPE="${BUILD_TYPE_CMAKE}" \
		-DCMAKE_INSTALL_PREFIX="${INSTALL_DIRECTORY}" \
		-B "${glfw_builddir}" -S "${glfw_source}" &&
		${bear_cmd} cmake --build "${glfw_builddir}" &&
		cmake --install "${glfw_builddir}" --prefix "${INSTALL_DIRECTORY}"
}

function build_cglm() {
	cglm_source="$(pwd)/subprojects/cglm"
	cglm_builddir="$(pwd)/build/subprojects/cglm"

	${bear_cmd} cmake \
		-DCMAKE_BUILD_TYPE="${BUILD_TYPE_CMAKE}" \
		-DCMAKE_INSTALL_PREFIX="${INSTALL_DIRECTORY}" \
		-B "${cglm_builddir}" -S "${cglm_source}" &&
		${bear_cmd} cmake --build "${cglm_builddir}" &&
		cmake --install "${cglm_builddir}" --prefix "${INSTALL_DIRECTORY}"
}

function build_cgltf() {
	cgltf_source="$(pwd)/cmake/cgltf"
	cgltf_builddir="$(pwd)/build/subprojects/cgltf"

	${bear_cmd} cmake \
		-DCMAKE_BUILD_TYPE="${BUILD_TYPE_CMAKE}" \
		-DCMAKE_INSTALL_PREFIX="${INSTALL_DIRECTORY}" \
		-B "${cgltf_builddir}" -S "${cgltf_source}" &&
		${bear_cmd} cmake --build "${cgltf_builddir}" &&
		cmake --install "${cgltf_builddir}" --prefix "${INSTALL_DIRECTORY}"
}

function build_miniaudio() {
	miniaudio_source="$(pwd)/subprojects/miniaudio"
	miniaudio_builddir="$(pwd)/build/subprojects/miniaudio"

	${bear_cmd} cmake \
		-DCMAKE_BUILD_TYPE="${BUILD_TYPE_CMAKE}" \
		-DCMAKE_INSTALL_PREFIX="${INSTALL_DIRECTORY}" \
		-B "${miniaudio_builddir}" -S "${miniaudio_source}" &&
		${bear_cmd} cmake --build "${miniaudio_builddir}" &&
		cmake --install "${miniaudio_builddir}" --prefix "${INSTALL_DIRECTORY}"
}

function build_freetype() {
	freetype_source="$(pwd)/subprojects/freetype"
	freetype_builddir="$(pwd)/build/subprojects/freetype"

	${bear_cmd} cmake \
		-DCMAKE_BUILD_TYPE="${BUILD_TYPE_CMAKE}" \
		-DCMAKE_INSTALL_PREFIX="${INSTALL_DIRECTORY}" \
		-B "${freetype_builddir}" -S "${freetype_source}" &&
		${bear_cmd} cmake --build "${freetype_builddir}" &&
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
		make -C "${ffmpeg_builddir}" clean &&
		${bear_cmd} make -C "${ffmpeg_builddir}" -j &&
		make -C "${ffmpeg_builddir}" install)
}

function build_libass() {
	libass_source="$(pwd)/subprojects/libass"
	libass_builddir="$(pwd)/build/subprojects/libass"

	meson setup \
		-Dbuildtype="${BUILD_TYPE_MESON}" \
		"${libass_builddir}" "${libass_source}" --prefix="${INSTALL_DIRECTORY}" &&
		${bear_cmd} meson compile -C "${libass_builddir}" &&
		meson install -C "${libass_builddir}"
}

function build_mpv() {
	mpv_source="$(pwd)/subprojects/mpv"
	mpv_builddir="$(pwd)/build/subprojects/mpv"

	meson setup \
		-Dbuildtype="${BUILD_TYPE_MESON}" \
		"${mpv_builddir}" "${mpv_source}" --prefix="${INSTALL_DIRECTORY}" &&
		${bear_cmd} meson compile -C "${mpv_builddir}" &&
		meson install -C "${mpv_builddir}"
}

git submodule update --init --recursive &&
	build_bear &&
	build_glad &&
	build_glfw &&
	build_sdl &&
	build_stb &&
	build_cglm &&
	build_cgltf &&
	build_freetype &&
	build_miniaudio &&
	build_ffmpeg &&
	build_libplacebo &&
	build_libass &&
	build_mpv
