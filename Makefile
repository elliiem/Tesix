
source := src/main.cpp include/*

build/main: $(source) build
	# clang src/main.cpp -o build/main -Iinclude -std=c++20 -lstdc++ -lm -g -Wall -fsanitize=address
	clang -D_DEBUG=1 src/main.cpp -o build/main -Iinclude -std=c++20 -lstdc++ -lm -g -Wall -fsanitize=address

build:
	mkdir build

run: build/main
	build/main
