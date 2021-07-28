#include "chip8.h"

#include <SDL2/SDL.h>


int main(int argc, char **argv)
{
    SDL_Window *window;
    SDL_Renderer *renderer;
    SDL_Texture *texture;

    struct chip8 c;
    chip8_init(&c, argv[1]);

    if (SDL_Init(SDL_INIT_VIDEO) < 0)
    {
        return (1);
    }

    window = SDL_CreateWindow("chip8", SDL_WINDOWPOS_UNDEFINED, SDL_WINDOWPOS_UNDEFINED, 1280, 640, SDL_WINDOW_SHOWN);
    renderer = SDL_CreateRenderer(window, -1, SDL_RENDERER_ACCELERATED | SDL_RENDERER_PRESENTVSYNC);
    texture = SDL_CreateTexture(renderer, SDL_PIXELFORMAT_RGBA8888, SDL_TEXTUREACCESS_STREAMING, 64, 32);


    unsigned int *pixels = malloc(64 * 32 * sizeof(unsigned int));
    memset(pixels, 0, 64 * 32 * sizeof(unsigned int));



    SDL_Event event;
    int quit = 0;

    while (!quit)
    {
        int index = 0;
        for (u8 i = 0; i < 32; i++)
        {
            for (u8 j = 0; j < 64; j++)
            {
                if (c.display[i * 64 + j])
                    pixels[index] = 0xFFFFFFFF;
                else
                    pixels[index] = 0x00000000;
                index++;
            }
        }


            SDL_UpdateTexture(texture, NULL, pixels, 64 * 4);
            SDL_RenderCopy(renderer, texture, NULL, NULL);
            SDL_RenderPresent(renderer);


        while(SDL_PollEvent(&event))
        {
            switch (event.type)
            {
                case SDL_QUIT: quit = SDL_TRUE; break;
                case SDL_KEYDOWN:
                    switch (event.key.keysym.sym)
                    {
                        case SDLK_1: c.keys[0x1] = 1; break;
                        case SDLK_2: c.keys[0x2] = 1; break;
                        case SDLK_3: c.keys[0x3] = 1; break;
                        case SDLK_4: c.keys[0xC] = 1; break;
                        case SDLK_q: c.keys[0x4] = 1; break;
                        case SDLK_w: c.keys[0x5] = 1; break;
                        case SDLK_e: c.keys[0x6] = 1; break;
                        case SDLK_r: c.keys[0xD] = 1; break;
                        case SDLK_a: c.keys[0x7] = 1; break;
                        case SDLK_s: c.keys[0x8] = 1; break;
                        case SDLK_d: c.keys[0x9] = 1; break;
                        case SDLK_f: c.keys[0xE] = 1; break;
                        case SDLK_z: c.keys[0xA] = 1; break;
                        case SDLK_x: c.keys[0x0] = 1; break;
                        case SDLK_c: c.keys[0xB] = 1; break;
                        case SDLK_v: c.keys[0xF] = 1; break;
                    }
                    break;
                case SDL_KEYUP:
                    switch (event.key.keysym.sym)
                    {
                        case SDLK_1: c.keys[0x1] = 0; break;
                        case SDLK_2: c.keys[0x2] = 0; break;
                        case SDLK_3: c.keys[0x3] = 0; break;
                        case SDLK_4: c.keys[0xC] = 0; break;
                        case SDLK_q: c.keys[0x4] = 0; break;
                        case SDLK_w: c.keys[0x5] = 0; break;
                        case SDLK_e: c.keys[0x6] = 0; break;
                        case SDLK_r: c.keys[0xD] = 0; break;
                        case SDLK_a: c.keys[0x7] = 0; break;
                        case SDLK_s: c.keys[0x8] = 0; break;
                        case SDLK_d: c.keys[0x9] = 0; break;
                        case SDLK_f: c.keys[0xE] = 0; break;
                        case SDLK_z: c.keys[0xA] = 0; break;
                        case SDLK_x: c.keys[0x0] = 0; break;
                        case SDLK_c: c.keys[0xB] = 0; break;
                        case SDLK_v: c.keys[0xF] = 0; break;
                    }
                    break;
            }
        }


        chip8_timer(&c);
        for(int i=0; i<(540.0f/60.0f); i++)
            chip8_cycle(&c);

    }


    free(pixels);

    chip8_destroy(&c);
    SDL_DestroyWindow(window);
    SDL_Quit();
    return 0;
}
