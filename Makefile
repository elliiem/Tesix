
source := examples/bouncing-box.cpp include/*

.PHONY build/bouncing-box: $(source) build
	clang -D_DEBUG=1 examples/bouncing-box.cpp -o build/bouncing-box -Iinclude -std=c++20 -lstdc++ -lm -g -Wall -fsanitize=address

.PHONY build/colors: $(source) build
	clang -D_DEBUG=1 examples/colors.cpp -o build/colors -Iinclude -std=c++20 -lstdc++ -lm -g -Wall -fsanitize=address

build:
	mkdir -p build

bouncing-box: build/bouncing-box
	build/bouncing-box

colors: build/colors
	build/colors
