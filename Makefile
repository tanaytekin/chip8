all: chip8

chip8: chip8.o chip8_sdl.o
	gcc -o2 -std=c99 -Wall -pedantic chip8.c chip8_sdl.c -o chip8 `sdl2-config --cflags --libs`

clean:
	rm -rf chip8.o chip8_sdl.o chip8
