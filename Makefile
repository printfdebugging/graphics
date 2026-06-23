run: debug
	./build/bin/editor

debug:
	cmake -DCMAKE_BUILD_TYPE=Debug -B build && cmake --build build

release:
	cmake -DCMAKE_BUILD_TYPE=Release -B build && cmake --build build

install: clean
	cmake -DCMAKE_INSTALL_PREFIX=install -DCMAKE_BUILD_TYPE=Release -B build && cmake --build build
	cmake --install build --prefix install

dependencies:
	./scripts/dependencies.sh

perf: debug
	perf record --debuginfod --call-graph dwarf,65528 ./build/bin/editor
	perf script -i perf.data | ./external/flamegraph/stackcollapse-perf.pl | ./external/flamegraph/flamegraph.pl \
		--width=2400 \
		--minwidth=2 \
		--height=20 \
		--bgcolors="#282c34" > flamegraph.svg
	chromium flamegraph.svg

reformat:
	find engine/* projects/* -iname '*.h' -o -iname '*.c' -o -iname '*.vert' -o -iname '*.frag' | xargs clang-format -i

windows:
	$(if $(shell which docker),docker build .,echo "docker not found")

mac:
	# todo: spin up a docker macos container and build the application there, just to check that everything compiles atleast
	echo "todo: test run in various docker containers"

clean:
	rm -rf build
	rm -rf install

renderdoc:
	LD_PRELOAD=/usr/lib/libasan.so qrenderdoc
