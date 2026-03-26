#!/bin/env bash

function build_bear() {
	bear_source="$(pwd)/subprojects/bear"
	bear_install_dir="$(pwd)/install/bin"
	(cd "${bear_source}" && cargo build --release)
	mkdir -p "${bear_install_dir}"
	install -m 755 ${bear_source}/target/release/{bear.exe,exec.dll,wrapper.exe} "${bear_install_dir}"
}

function setup_environment() {
	export PKG_CONFIG_PATH=$(pwd)/install/lib/pkgconfig

	if [ -f "/etc/os-release" ]; then
		. /etc/os-release
		case "$ID" in
		'msys2') {
			export BEAR_EXE="$(pwd)/install/bin/bear.exe"
			export BEAR_CONFIG_INTERCEPT_MODE=wrapper
			export BEAR_CONFIG_INTERCEPT_PATH="path: $(cygpath.exe -w $(pwd)/install/bin/wrapper.exe)"
			export BEAR_COMPILE_COMMANDS_FILE="$(pwd)/compile_commands.json"
			export BEAR_CONFIG_FILE="$(pwd)/bear.yml"
		} ;;
		'arch') {
			export BEAR_EXE="$(which bear)"
			export BEAR_CONFIG_INTERCEPT_MODE=preload
			export BEAR_CONFIG_INTERCEPT_PATH=""
			export BEAR_COMPILE_COMMANDS_FILE="$(pwd)/compile_commands.json"
			export BEAR_CONFIG_FILE="$(pwd)/bear.yml"
		} ;;
		*) {
			echo "distribution not supported" && exit 1
		} ;;
		esac

		export BEAR_CMD="${BEAR_EXE} --config ${BEAR_CONFIG_FILE} --output ${BEAR_COMPILE_COMMANDS_FILE} --append -- "
		export INSTALL_DIRECTORY="$(pwd)/install"
	else
		echo "/etc/os-release not found" && exit 1
	fi
}

function config_bear() {
	echo "schema: 4.0
intercept:
  mode: ${BEAR_CONFIG_INTERCEPT_MODE}
  ${BEAR_CONFIG_INTERCEPT_PATH}

format:
  paths:
    directory: absolute
    file: absolute
  entries:
    use_array_format: true
    include_output_field: true" >"${BEAR_CONFIG_FILE}"
}

function build_glad() {
	glad_source="$(pwd)/subprojects/glad"
	glad_builddir="$(pwd)/build/subprojects/glad"

	${BEAR_CMD} cmake -DCMAKE_INSTALL_PREFIX="${INSTALL_DIRECTORY}" -B "${glad_builddir}" -S "${glad_source}" &&
		${BEAR_CMD} cmake --build "${glad_builddir}" &&
		cmake --install "${glad_builddir}" --prefix "${INSTALL_DIRECTORY}"
}

function build_libplacebo() {
	libplacebo_source="$(pwd)/subprojects/libplacebo"
	libplacebo_builddir="$(pwd)/build/subprojects/libplacebo"

	meson setup \
		-Dgl-proc-addr=disabled \
		-Dvk-proc-addr=disabled \
		"${libplacebo_builddir}" "${libplacebo_source}" --prefix="${INSTALL_DIRECTORY}" &&
		${BEAR_CMD} meson compile -C "${libplacebo_builddir}" &&
		meson install -C "${libplacebo_builddir}"
}

function build_glfw() {
	glfw_source="$(pwd)/subprojects/glfw"
	glfw_builddir="$(pwd)/build/subprojects/glfw"

	${BEAR_CMD} cmake -DCMAKE_INSTALL_PREFIX="${INSTALL_DIRECTORY}" -B "${glfw_builddir}" -S "${glfw_source}" &&
		${BEAR_CMD} cmake --build "${glfw_builddir}" &&
		cmake --install "${glfw_builddir}" --prefix "${INSTALL_DIRECTORY}"
}

function build_stb() {
	glfw_source="$(pwd)/cmake/stb"
	glfw_builddir="$(pwd)/build/subprojects/stb"

	${BEAR_CMD} cmake -DCMAKE_INSTALL_PREFIX="${INSTALL_DIRECTORY}" -B "${glfw_builddir}" -S "${glfw_source}" &&
		${BEAR_CMD} cmake --build "${glfw_builddir}" &&
		cmake --install "${glfw_builddir}" --prefix "${INSTALL_DIRECTORY}"
}

function build_cglm() {
	cglm_source="$(pwd)/subprojects/cglm"
	cglm_builddir="$(pwd)/build/subprojects/cglm"

	${BEAR_CMD} cmake -DCMAKE_INSTALL_PREFIX="${INSTALL_DIRECTORY}" -B "${cglm_builddir}" -S "${cglm_source}" &&
		${BEAR_CMD} cmake --build "${cglm_builddir}" &&
		cmake --install "${cglm_builddir}" --prefix "${INSTALL_DIRECTORY}"
}

function build_cgltf() {
	cgltf_source="$(pwd)/cmake/cgltf"
	cgltf_builddir="$(pwd)/build/subprojects/cgltf"

	${BEAR_CMD} cmake -DCMAKE_INSTALL_PREFIX="${INSTALL_DIRECTORY}" -B "${cgltf_builddir}" -S "${cgltf_source}" &&
		${BEAR_CMD} cmake --build "${cgltf_builddir}" &&
		cmake --install "${cgltf_builddir}" --prefix "${INSTALL_DIRECTORY}"
}

function build_miniaudio() {
	miniaudio_source="$(pwd)/subprojects/miniaudio"
	miniaudio_builddir="$(pwd)/build/subprojects/miniaudio"

	${BEAR_CMD} cmake -DCMAKE_INSTALL_PREFIX="${INSTALL_DIRECTORY}" -B "${miniaudio_builddir}" -S "${miniaudio_source}" &&
		${BEAR_CMD} cmake --build "${miniaudio_builddir}" &&
		cmake --install "${miniaudio_builddir}" --prefix "${INSTALL_DIRECTORY}"
}

function build_freetype() {
	freetype_source="$(pwd)/subprojects/freetype"
	freetype_builddir="$(pwd)/build/subprojects/freetype"

	${BEAR_CMD} cmake -DCMAKE_INSTALL_PREFIX="${INSTALL_DIRECTORY}" -B "${freetype_builddir}" -S "${freetype_source}" &&
		${BEAR_CMD} cmake --build "${freetype_builddir}" &&
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
		${BEAR_CMD} make -C "${ffmpeg_builddir}" -j &&
		make -C "${ffmpeg_builddir}" install)
}

function build_libass() {
	libass_source="$(pwd)/subprojects/libass"
	libass_builddir="$(pwd)/build/subprojects/libass"
	echo "${PKG_CONFIG_PATH}"
	meson setup "${libass_builddir}" "${libass_source}" --prefix="${INSTALL_DIRECTORY}" &&
		${BEAR_CMD} meson compile -C "${libass_builddir}" &&
		meson install -C "${libass_builddir}"
}

function build_mpv() {
	mpv_source="$(pwd)/subprojects/mpv"
	mpv_builddir="$(pwd)/build/subprojects/mpv"

	meson setup "${mpv_builddir}" "${mpv_source}" --prefix="${INSTALL_DIRECTORY}" &&
		${BEAR_CMD} meson compile -C "${mpv_builddir}" &&
		meson install -C "${mpv_builddir}"
}

# todo 0: avoid building ffmpeg if it's already built
# done 1: pkgconfig for glad, stb, cgltf
# done 2: point to the installdir pkgconfig
# todo 3: create switches for various build parameters like BUILD_SHARED_LIBS becomes --cmake-build-shared-libs or something similar
# todo 4: build the application with these libraries

git submodule update --init --recursive &&
	setup_environment &&
	build_bear &&
	config_bear &&
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
