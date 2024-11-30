#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <time.h>
#include "include/core.h"

Emu init() {
    srand(time(NULL));
    Emu e = {
        .pc = PROGRAM_START,
        .memory = {
            0xF0, 0x90, 0x90, 0x90, 0xF0, // 0
            0x20, 0x60, 0x20, 0x20, 0x70, // 1
            0xF0, 0x10, 0xF0, 0x80, 0xF0, // 2
            0xF0, 0x10, 0xF0, 0x10, 0xF0, // 3
            0x90, 0x90, 0xF0, 0x10, 0x10, // 4
            0xF0, 0x80, 0xF0, 0x10, 0xF0, // 5
            0xF0, 0x80, 0xF0, 0x90, 0xF0, // 6
            0xF0, 0x10, 0x20, 0x40, 0x40, // 7
            0xF0, 0x90, 0xF0, 0x90, 0xF0, // 8
            0xF0, 0x90, 0xF0, 0x10, 0xF0, // 9
            0xF0, 0x90, 0xF0, 0x90, 0x90, // A
            0xE0, 0x90, 0xE0, 0x90, 0xE0, // B
            0xF0, 0x80, 0x80, 0x80, 0xF0, // C
            0xE0, 0x90, 0x90, 0x90, 0xE0, // D
            0xF0, 0x80, 0xF0, 0x80, 0xF0, // E
            0xF0, 0x80, 0xF0, 0x80, 0x80 // F
        }
    };
    return e;
}

void push(Emu *e, uint16_t return_address) {
    if (e->sp < STACK_SIZE - 1) e->stack[++e->sp] = return_address;
}

uint16_t pop(Emu *e) {
    return e->stack[e->sp--];
}

uint16_t fetch(Emu *e) {
    uint16_t instruction =
        (((uint16_t) e->memory[e->pc]) << 8) | (uint16_t) e->memory[e->pc+1];
    e->pc += 2;
    return instruction;
}

void draw(Emu *e, uint8_t x, uint8_t y, uint8_t n) {
    uint16_t addr = e->i_reg;
    uint8_t flipped = 0;
    for (int i = 0; i < n; i++, y++) {
        y %= DISPLAY_HEIGHT;
        for (int shift = 0; shift < 8; shift++) {
            if (e->memory[addr + i] & (0x80 >> shift)) {
                e->display[y] ^= MASK >> ((x + shift) % DISPLAY_WIDTH);
                flipped = 1;
            }
        }
    }

    if (flipped) {
        e->registers[0xf] = 1;
        e->display_update = true;
    }
}

void cls(Emu *e) {
    memset(e->display, 0, DISPLAY_HEIGHT * sizeof(uint64_t));
}

void operations(Emu *e, uint16_t instruction) {
    uint8_t x = (instruction & 0x0F00) >> 8;
    uint8_t y = (instruction & 0x00F0) >> 4;
    
    uint8_t flag = 0;
    switch (instruction & 0x000F) {
        case 0x0:
            e->registers[x] = e->registers[y];
            break;
        case 0x1:
            e->registers[x] |= e->registers[y];
            break;
        case 0x2:
            e->registers[x] &= e->registers[y];
            break;
        case 0x3:
            e->registers[x] ^= e->registers[y];
            break;
        case 0x4:
            e->registers[x] += e->registers[y];
            e->registers[0xf] = e->registers[x] < e->registers[y];
            break;
        case 0x5:
            flag = e->registers[x] >= e->registers[y];
            e->registers[x] -= e->registers[y];
            e->registers[0xf] = flag;
            break;
        case 0x6:
            flag = e->registers[x] & 1;
            e->registers[x] /= 2;
            e->registers[0xf] = flag;
            break;
        case 0x7:
            flag = e->registers[y] >= e->registers[x];
            e->registers[x] =  e->registers[y] - e->registers[x];
            e->registers[0xf] = flag;
            break;
        case 0xE:
            flag = e->registers[x] >> 7;
            e->registers[x] *= 2;
            e->registers[0xf] = flag;
            break;
    }
}

void f_operations(Emu *e, uint16_t instruction) {
    uint8_t x = (instruction & 0x0f00) >> 8;
    switch (instruction & 0x00ff) {
        case 0x07:
            e->registers[x] = e->dt;
            break;
        case 0x0A:
            if (!e->keys) { e->pc -= 2; break; }
            for (uint8_t i = 0; i < 16; i++) {
                if ((MASK_16 >> i) == e->keys) e->registers[x] = i;
            }
            break;
        case 0x15:
            e->dt = e->registers[x];
            break;
        case 0x18:
            e->st = e->registers[x];
            break;
        case 0x1e:
            e->i_reg += e->registers[x];
            break;
        case 0x29:
            e->i_reg = 5 * e->registers[x];
            break;
        case 0x33:
            e->memory[e->i_reg] = e->registers[x] / 100;
            e->memory[e->i_reg+1] = (e->registers[x] % 100) / 10;
            e->memory[e->i_reg+2] = e->registers[x] % 10;
            break;
        case 0x55:
            for (uint8_t i = 0; i <= x; i++)
                e->memory[e->i_reg+i] = e->registers[i];
            break;
        case 0x65:
            for (uint8_t i = 0; i <= x; i++)
                e->registers[i] = e->memory[e->i_reg+i];
            break;
    }
}

void execute(Emu *e, uint16_t instruction) {
    switch (instruction & 0xF000) {
        case 0x0000:
            if (instruction & 0x000F) e->pc = pop(e);
            else if ((instruction & 0x00F0) == 0x00E0) cls(e);
            break;
        case 0x1000:
            e->pc = instruction & 0x0FFF;
            break;
        case 0x2000:
            push(e, e->pc);
            e->pc = instruction & 0x0FFF;
            break;
        case 0x3000: // 3xkk
            if (e->registers[(instruction & 0x0F00) >> 8] == (instruction & 0x00FF)) e->pc += 2;
            break;
        case 0x4000:
            if (e->registers[(instruction & 0x0F00) >> 8] != (instruction & 0x00FF)) e->pc += 2;
            break;
        case 0x5000: // 5xy0
            if (e->registers[(instruction & 0x0F00) >> 8] == e->registers[(instruction & 0x00F0) >> 4]) e->pc += 2;
            break;
        case 0x6000: // 6xkk LD Vx, kk
            e->registers[(instruction & 0x0F00) >> 8] = instruction & 0x00FF;
            break;
        case 0x7000: // 7xkk ADD Vx, kk
            e->registers[(instruction & 0x0F00) >> 8] += instruction & 0x00FF;
            break;
        case 0x8000:
            operations(e, instruction);
            break;
        case 0x9000: // 9xy0
            if (e->registers[(instruction & 0x0F00) >> 8] != e->registers[(instruction & 0x00F0) >> 4]) e->pc += 2;
            break;
        case 0xA000: // annn
            e->i_reg = instruction & 0x0FFF;
            break;
        case 0xB000: // bnnn: jump to v0 + nnn
            e->pc = e->registers[0] + (instruction & 0x0FFF);
            break;
        case 0xC000: // cxkk
            e->registers[(instruction & 0x0F00) >> 8] =
                ((uint8_t) rand()) & (instruction & 0x00FF);
            break;
        case 0xD000: // dxyn
            draw(
                e,
                e->registers[(instruction & 0x0F00) >> 8],
                e->registers[(instruction & 0x00F0) >> 4],
                instruction & 0x000F
            );
            break;
        case 0xF000:
            f_operations(e, instruction);
            break;
        case 0xE000: // ex9e
            uint8_t key = e->registers[(instruction & 0x0F00) >> 8];
            if (((instruction & 0x00FF) == 0x9E && (e->keys & (MASK_16 >> key))) ||
                ((instruction & 0x00FF) == 0xA1 && !(e->keys & (MASK_16 >> key)))) { e->pc += 2; }
            break;
    }
}

void tick(Emu *e) {
    e->display_update = false;
    uint16_t instruction = fetch(e);
    execute(e, instruction);
}

void decrement_timers(Emu *e) {
    if (e->st) e->st--;
    if (e->dt) e->dt--;
}