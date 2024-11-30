#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <pthread.h>
#include "SDL2/SDL.h"
#include "SDL2/SDL_keycode.h"
#include "SDL2/SDL_timer.h"
#include "include/core.h"

void *decrement_registers(void *arguments) { 
    Emu *e = (Emu*) arguments;
    while (1) {
        SDL_Delay(20);
        if (e->dt) e->dt--;
        if (e->st) e->st--;
    }
}

Emu start(char *filename) {
    FILE *rom = fopen(filename, "r");

    if (rom == NULL) {
        fprintf(stderr, "Error: could not open file.\n");
        exit(1);
    }

    Emu e = init();
    size_t bytes_read = fread(
        e.memory+PROGRAM_START,
        sizeof(uint8_t),
        MEMORY_SIZE,
        rom);
    
    fclose(rom);

    if (bytes_read == 0) {
        fprintf(stderr, "Couldn't read from file.\n");
        exit(1);
    }

    return e;
}

void draw_screen(Emu *e, SDL_Renderer *renderer) {
    SDL_SetRenderDrawColor(renderer, 0, 0, 0, 255);
    SDL_RenderClear(renderer);
    
    SDL_SetRenderDrawColor(renderer, 102, 255, 102 , 255);
    for (uint8_t line = 0; line < DISPLAY_HEIGHT; line++)
        for (uint8_t column = 0; column < DISPLAY_WIDTH; column++)
            if (e->display[line] & (MASK >> column)) {
                SDL_Rect point = {column * PIXEL_SIZE, line * PIXEL_SIZE, PIXEL_SIZE, PIXEL_SIZE};
                
                SDL_RenderFillRect(renderer, &point);
            }

    // SDL_SetRenderDrawColor(renderer, 0, 0, 0 , 255);
    
    SDL_RenderPresent(renderer);
}

uint8_t get_key(SDL_Keysym key) {
    if (key.sym >= SDLK_1 && key.sym < SDLK_4)
        return key.sym - SDLK_0;
    switch (key.sym) {
        case SDLK_4: return 0xc;
        case SDLK_q: return 0x4;
        case SDLK_w: return 0x5;
        case SDLK_e: return 0x6;
        case SDLK_r: return 0xd;
        case SDLK_a: return 0x7;
        case SDLK_s: return 0x8;
        case SDLK_d: return 0x9;
        case SDLK_f: return 0xe;
        case SDLK_z: return 0xa;
        case SDLK_x: return 0x0;
        case SDLK_c: return 0xb;
        case SDLK_v: return 0xf;
        default: return 0x10;
    }
}

int main(int argc, char *argv[]) {
    if (argc < 2) {
        fprintf(stderr, "No file name given.\nUsage: %s <filename>\n", argv[0]);
    }

    Emu e = start(argv[1]);

    if (SDL_Init(SDL_INIT_EVERYTHING)) {
        fprintf(stderr, "Could not initialize SDL.\n");
        exit(1);
    }

    SDL_Window *window = SDL_CreateWindow(
        argv[1],
        SDL_WINDOWPOS_CENTERED,
        SDL_WINDOWPOS_CENTERED,
        DISPLAY_WIDTH * PIXEL_SIZE,
        DISPLAY_HEIGHT * PIXEL_SIZE,
        SDL_WINDOW_OPENGL);

    SDL_Renderer *renderer = SDL_CreateRenderer(
        window,
        -1,
        0);
    pthread_t threads[1];
    pthread_create(&threads[0], NULL, decrement_registers, &e);

    SDL_Event event;

    while (1) {
        if (SDL_PollEvent(&event)) {
            if (event.type == SDL_QUIT) break;
            else if (event.type == SDL_KEYDOWN){
                e.keys = MASK_16 >> get_key(event.key.keysym);
            }
        }

        // for (int i = 0; i < 10; i++)
        tick(&e);
        // SDL_Delay(5);
        draw_screen(&e, renderer);
        // decrement_timers(&e);
    }
    SDL_Quit();

    return 0;
}