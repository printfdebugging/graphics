include ./scripts/make/exports.mk
include ./scripts/make/build_wrappers.mk
include ./scripts/make/system_dependencies.mk

run: debug
	./build/bin/editor

$(eval $(call build_wrapper_meson,fontconfig))
$(eval $(call build_wrapper_cmake,cglm))
$(eval $(call build_wrapper_cmake,freetype))
$(eval $(call build_wrapper_meson,harfbuzz))
$(eval $(call build_wrapper_cmake,glfw))
$(eval $(call build_wrapper_cmake,glad))
$(eval $(call build_wrapper_autotools,ffmpeg))

externals: \
	glfw \
	harfbuzz \
	glad \
	cglm \
	fontconfig

debug: clangd
	cmake \
		-DCMAKE_BUILD_TYPE=Debug \
		-B build && cmake --build build

clangd:
	$(call clangd_config_string,engine,$(BUILD_DIRECTORY))
	$(call clangd_config_string,projects,$(BUILD_DIRECTORY))

release:
	cmake \
		-DCMAKE_BUILD_TYPE=Release \
		-B build && cmake --build build

# todo: fix installs
install:
	cmake \
		-DCMAKE_BUILD_TYPE=Release \
		-DCMAKE_INSTALL_PREFIX=install \
		-B build && cmake --build build

	cmake --install build --prefix install

dependencies:
	./scripts/dependencies.sh

perf: debug
	perf record \
		--debuginfod \
		--call-graph dwarf,2048 \
		./build/bin/editor

	perf script -i perf.data \
		| ./external/flamegraph/stackcollapse-perf.pl \
		| ./external/flamegraph/flamegraph.pl \
		--width=2400 \
		--minwidth=2 \
		--height=20 \
		--bgcolors="#282c34" > flamegraph.svg

	chromium flamegraph.svg

reformat:
	find \
		engine/* \
		projects/* \
		-iname '*.h' -o \
		-iname '*.c' -o \
		-iname '*.vert' -o \
		-iname '*.frag' | xargs clang-format -i

clean:
	rm -rf build
	rm -rf install
	rm .clangd
