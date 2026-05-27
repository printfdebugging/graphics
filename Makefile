run: 
	cmake -B build && cmake --build build && ./build/executable

clean:
	rm -rf build
