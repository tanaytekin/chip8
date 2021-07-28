#include <stdio.h>
#include <stdlib.h>
#include <memory.h>
#include <time.h>

#include "chip8.h"

static const u8 font[] =
        {
                /*0*/  0xF0,0x90,0x90,0x90,0xF0,
                /*1*/  0x20,0x60,0x20,0x20,0x70,
                /*2*/  0xF0,0x10,0xF0,0x80,0xF0,
                /*3*/  0xF0,0x10,0xF0,0x10,0xF0,
                /*4*/  0x90,0x90,0xF0,0x10,0x10,
                /*5*/  0xF0,0x80,0xF0,0x10,0xF0,
                /*6*/  0xF0,0x80,0xF0,0x90,0xF0,
                /*7*/  0xF0,0x10,0x20,0x40,0x40,
                /*8*/  0xF0,0x90,0xF0,0x90,0xF0,
                /*9*/  0xF0,0x90,0xF0,0x10,0xF0,
                /*A*/  0xF0,0x90,0xF0,0x90,0x90,
                /*B*/  0xE0,0x90,0xE0,0x90,0xE0,
                /*C*/  0xF0,0x80,0x80,0x80,0xF0,
                /*D*/  0xE0,0x90,0x90,0x90,0xE0,
                /*E*/  0xF0,0x80,0xF0,0x80,0xF0,
                /*F*/  0xF0,0x80,0xF0,0x80,0x80
        };

void chip8_init(struct chip8 *c, const char *path)
{
    memset(c, 0, sizeof(struct chip8));
    c->memory = malloc(4 * 1024);
    memcpy(c->memory, font, sizeof(font));
    srand(time(NULL));
    chip8_load(c, path);
}

void chip8_destroy(struct chip8 *c)
{
    free(c->memory);
}


void chip8_load(struct chip8 *c, const char *path)
{
    FILE *fp;
    size_t size;

    fp = fopen(path, "r");

    if(fp == NULL)
    {
        fprintf(stderr, "Failed to open file :%s", path);
        return;
    }


    fseek(fp, 0L, SEEK_END);
    size = ftell(fp);
    fseek(fp, 0L, SEEK_SET);

    if(size > 4*1024)
    {
        fprintf(stderr, "Program is too big.");
        return;
    }

    fread(c->memory + 0x200, size, 1, fp);
    fclose(fp);

    c->pc = 0x200;
}

#define N (opcode & 0x000F)
#define NNN (opcode & 0x0FFF)
#define X ((opcode & 0x0F00) >> 8)
#define Y ((opcode & 0x00F0) >> 4)
#define KK (opcode & 0x00FF)


void chip8_cycle(struct chip8 *c)
{
    u16 opcode = (c->memory[c->pc] << 8) | (c->memory[c->pc+1]);

    c->pc += 2;

    switch((opcode & 0xF000) >> 12)
    {

        case 0:
            switch(KK)
            {
                case 0xE0:
                    memset(c->display, 0, 64*32* sizeof(u8));
                    break;
                case 0xEE:
                    c->pc = c->stack[c->sp--];
                    break;
            }
            break;
        case 1:
            c->pc = NNN;
            break;
        case 2:
            c->stack[++c->sp] = c->pc;
            c->pc = NNN;
            break;
        case 3:
            if(c->V[X] == KK)
                c->pc += 2;
            break;
        case 4:
            if(c->V[X] != KK)
                c->pc += 2;
            break;
        case 5:
            if(N == 0)
                if(c->V[X] == c->V[Y])
                    c->pc += 2;
            break;
        case 6:
            c->V[X] = KK;
        break;
        case 7:
            c->V[X] += KK;
            break;
        case 8:
            switch(N)
            {
                case 0:
                    c->V[X] = c->V[Y];
                    break;
                case 1:
                    c->V[X] |= c->V[Y];
                    break;
                case 2:
                    c->V[X] &= c->V[Y];
                    break;
                case 3:
                    c->V[X] ^= c->V[Y];
                    break;
                case 4:
                {
                    u16 res = c->V[X] + c->V[Y];
                    if(res > 0xFF)
                        c->V[0xF] = 1;
                    else
                        c->V[0xF] = 0;
                    c->V[X] = res & 0x00FF;
                }
                break;
                case 5:
                    if(c->V[X] > c->V[Y])
                        c->V[0xF] = 1;
                    else
                        c->V[0xF] = 0;
                    c->V[X] -= c->V[Y];
                    break;
                case 6:
                    c->V[0xF] = c->V[X] & 1;
                    c->V[X] >>= 1;
                    break;
                case 7:
                    if(c->V[Y] > c->V[X])
                        c->V[0xF] = 1;
                    else
                        c->V[0xF] = 0;
                    c->V[X] = c->V[Y] - c->V[X];
                    break;
                case 0xE:
                    c->V[0xF] = c->V[X] >> 7;
                    c->V[X] <<= 1;
                    break;

            }
            break;

        case 9:
            if(N == 0)
                if(c->V[X] != c->V[Y])
                    c->pc += 2;
            break;

        case 0xA:
            c->I = NNN;
            break;

        case 0xB:
            c->pc = NNN + c->V[0];
            break;

        case 0xC:
            c->V[X] = rand() & KK;
            break;

        case 0xD:
        {
            u8 x = c->V[X];
            u8 y = c->V[Y];
            c->V[0xF] = 0;

            for(u8 n=0; n<N; n++)
            {
                u8 byte = c->memory[c->I + n];
                for(int i=7; i>=0; i--)
                {
                    u8 bit = (byte>>i)&1;
                    u16 index =(x + (7-i))%64 + 64*((y+n)%32);

                    if(c->display[index] && bit)
                        c->V[0xF] = 1;

                    c->display[index]  ^= bit;
                }
            }

        }
            break;
        case 0xE:
            switch(KK)
            {
                case 0x9E:
                    if(c->keys[c->V[X]])
                        c->pc += 2;
                    break;
                case 0xA1:
                    if(!c->keys[c->V[X]])
                        c->pc += 2;
                    break;
            }
            break;

        case 0xF:
            switch(KK)
            {
                case 0x07:
                    c->V[X] = c->delay_timer;
                    break;
                case 0x0A:
                {
                    c->pc -= 2;
                    for(u8 i=0; i<16; i++)
                    {
                        if(c->keys[i])
                        {
                            c->V[X] = i;
                            c->pc +=2;
                            break;
                        }
                    }
                }
                    break;
                case 0x15:
                    c->delay_timer = c->V[X];
                    break;
                case 0x18:
                    c->sound_timer = c->V[X];
                    break;
                case 0x1E:
                    c->I += c->V[X];
                    break;
                case 0x29:
                    c->I = c->V[X]*5;
                    break;
                case 0x33:
                    c->memory[c->I] = (c->V[X]/100) % 10;
                    c->memory[c->I+1] = (c->V[X] / 10) % 10;
                    c->memory[c->I+2] = c->V[X] % 10;
                    break;
                case 0x55:
                    memcpy(&c->memory[c->I], &c->V[0], (X+1) * sizeof(u8));
                    break;
                case 0x65:
                    memcpy(&c->V[0], &c->memory[c->I], (X+1) * sizeof(u8));
                    break;
            }
            break;
    }


}

void chip8_timer(struct chip8 *c)
{
    if(c->delay_timer > 0)
        c->delay_timer--;
    if(c->sound_timer > 0)
        c->sound_timer--;
}