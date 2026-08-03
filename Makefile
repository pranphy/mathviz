all:
	g++ -std=c++23 -Iglad/include  -o bin/mest src/mest.cpp glad/src/gl.c -lGL -lglfw -lavformat -lavcodec -lswscale -l avutil
