run: debug
	./build/executable

debug:
	cmake -DCMAKE_BUILD_TYPE=Debug -B build && cmake --build build

# release:
# package: clean release


clean:
	rm -rf build

renderdoc:
	LD_PRELOAD=/usr/lib/libasan.so qrenderdoc
