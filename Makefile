run: debug
	./build/projects/game/game

debug:
	CC="ccache gcc" CXX="ccache g++" cmake -DCMAKE_BUILD_TYPE=Debug -B build && cmake --build build

# release:
# package: clean release

reformat:
	find engine/* projects/* -iname '*.h' -o -iname '*.c' | xargs clang-format -i

clean:
	rm -rf build

renderdoc:
	LD_PRELOAD=/usr/lib/libasan.so qrenderdoc
