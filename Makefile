run: 
	cmake -B build && cmake --build build && ./build/executable

clean:
	rm -rf build

renderdoc:
	LD_PRELOAD=/usr/lib/libasan.so qrenderdoc
