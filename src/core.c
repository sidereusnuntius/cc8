#include "include/core.h"

Emu init() {
    Emu e = {
        .pc = PROGRAM_START,
    };
    return e;
}

void push(Emu *e, uint16_t return_address) {
    if (e->sp < STACK_SIZE - 1) e->stack[++e->sp] = return_address;
}

uint16_t pop(Emu *e) {
    return e->stack[e->sp--];
}