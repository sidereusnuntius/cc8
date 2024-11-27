#include "include/core.h"

Emu init() {
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

void execute(Emu *e, uint16_t instruction) {
    switch (instruction & 0xF000) {
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
        case 0x9000:
            
            break;
        case 0xA000:
            
            break;
        case 0xB000:
            
            break;
        case 0xC000:
            
            break;
        case 0xD000:
            
            break;
        case 0xE000:
            
            break;
    }
}

void tick(Emu *e) {
    uint16_t instruction = fetch(e);
    execute(e, instruction);
}