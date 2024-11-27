#include <stdio.h>
#include <assert.h>
#include "include/core.h"

void test_stack() {
    Emu e = init();
    
    push(&e, 0x0000);
    push(&e, 0xF2);
    assert(e.sp == 2);
    assert(pop(&e) == 0xF2);
    assert(pop(&e) == 0);
    assert(e.sp == 0);
}

void test_jp() {
    // printf("\tjump\n");
    Emu e = init();

    e.memory[512] = 0x12;
    e.memory[513] = 0xAE;
    tick(&e);
    assert(e.pc == 0x2AE);
}

void test_call() {
    // printf("\tcall subroutine\n");
    Emu e = init();

    e.memory[512] = 0x22;
    e.memory[513] = 0xF6;
    tick(&e);
    assert(e.pc == 0x2F6);
    assert(e.sp == 1);
    assert(e.stack[e.sp] == 0x202);
}

// Tests 3xkk, which skips next instruction if Vx == kk.
void test_se_bytes() {
    Emu e = init();

    e.memory[512] = 0x33;
    e.memory[513] = 0x13;
    e.registers[3] = 0x13;
    
    tick(&e);

    assert(e.pc == 516);

    e.memory[516] = 0x31;
    e.memory[517] = 0x09;
    e.registers[1] = 0xAF;

    tick(&e);
    
    assert(e.pc == 518);
}

// Tests 4xkk, which skips next instruction if Vx != kk.
void test_sne_bytes() {
    Emu e = init();

    e.memory[512] = 0x44;
    e.memory[513] = 0x13;
    e.registers[4] = 0x13;
    
    tick(&e);

    assert(e.pc == 514);

    e.memory[514] = 0x41;
    e.memory[515] = 0x09;
    e.registers[1] = 0xAF;

    tick(&e);
    
    assert(e.pc == 518);
}

// 5xy0: skip next instruction if Vx == Vy.
void test_se_regs() {
    Emu e = init();
    e.registers[0] = 0x15;
    e.registers[9] = 0x15;
    e.registers[3] = 0x10;

    e.memory[512] = 0x50;
    e.memory[513] = 0x90;

    e.memory[516] = 0x50;
    e.memory[517] = 0x30;

    tick(&e);
    assert(e.pc == 516);

    tick(&e);
    assert(e.pc == 518);
}

// 6xkk: Vx = kk
void test_load_byte() {
    Emu e = init();

    e.memory[512] = 0x65;
    e.memory[513] = 0x24;
    e.memory[514] = 0x60;
    e.memory[515] = 0x10;

    tick(&e);
    tick(&e);

    assert(e.registers[0] == 0x10);
    assert(e.registers[5] == 0x24);
}

// 7xkk: Vx = Vx + kk
void test_add() {
    Emu e = init();
    e.registers[0] = 0x5;
    e.registers[7] = 0x10;

    e.memory[512] = 0x70;
    e.memory[513] = 0x0A;
    e.memory[514] = 0x77;
    e.memory[515] = 0x3F;
    
    tick(&e);
    tick(&e);

    assert(e.registers[0] == 0xF);
    assert(e.registers[7] == 0x4F);
}

int main() {
    printf("Testing the call stack...\n");
    test_stack();

    printf("Testing individual instructions...\n");
    test_jp();
    test_call();
    test_se_bytes();
    test_sne_bytes();
    test_se_regs();
    test_load_byte();
    test_add();

    printf("All tests were successful.\n");
    return 0;
}