run: debug
	./build/bin/game

debug:
	cmake -DCMAKE_BUILD_TYPE=Debug -B build && cmake --build build

release:
	cmake -DCMAKE_BUILD_TYPE=Release -B build && cmake --build build

install: clean
	cmake -DCMAKE_INSTALL_PREFIX=install -DCMAKE_BUILD_TYPE=Release -B build && cmake --build build
	cmake --install build --prefix install


reformat:
	find engine/* projects/* -iname '*.h' -o -iname '*.c' -o -iname '*.vert' -o -iname '*.frag' | xargs clang-format -i

clean:
	rm -rf build
	rm -rf install

renderdoc:
	LD_PRELOAD=/usr/lib/libasan.so qrenderdoc
