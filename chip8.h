#ifndef CHIP_8_H
#define CHIP_8_H

#include <stdint.h>

typedef uint8_t u8;
typedef uint16_t u16;

struct chip8
{
	u8 V[16];
	u16 I;
	u8 *memory;
	u8 display[64*32];
	u16 stack[16];
	u8 keys[16];
	u8 sp;
	u16 pc;
	u8 sound_timer;
	u8 delay_timer;
};


void chip8_init(struct chip8 *c, const char *path);
void chip8_load(struct chip8 *c, const char *path);
void chip8_destroy(struct chip8 *c);
void chip8_cycle(struct chip8 *c);
void chip8_timer(struct chip8 *c);

#endif