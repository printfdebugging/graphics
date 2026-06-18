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
