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

// 9xy0: Skip next instruction if Vx == Vy.
void test_sne_regs() {
    Emu e = init();
    e.registers[0xA] = 0x17;
    e.registers[5] = 0x2A;

    e.registers[0xE] = 0x1;
    e.registers[0xD] = 0x1;

    e.memory[512] = 0x9A;
    e.memory[513] = 0x50;

    e.memory[516] = 0x9E;
    e.memory[517] = 0xD0;

    tick(&e);
    assert(e.pc == 516);
    tick(&e);
    assert(e.pc == 518);

}

// annn: i = nnn
void test_ld_addr() {
    Emu e = init();
    e.memory[512] = 0xab;
    e.memory[513] = 0x24; // LD I, 0xB24

    tick(&e);
    assert(e.i_reg == 0xb24);
}

// bnnn: jump to nnn + v0.
void test_jp_addr() {
    Emu e = init();
    uint8_t v0 = 0x5;
    uint16_t addr = 0x2a3;
    e.registers[0] = v0;
    e.memory[512] = 0xb2;
    e.memory[513] = 0xa3;

    tick(&e);
    assert(e.pc == addr + v0);
}

// cxkk: vx = kk AND random byte.
void test_rand_byte() {
    Emu e = init();
    e.memory[512] = 0xc0;
    e.memory[513] = 0xFF;
    e.memory[514] = 0xc1;
    e.memory[515] = 0xFF;
    e.memory[516] = 0xc2;
    e.memory[517] = 0xFF;

    tick(&e);
    tick(&e);
    tick(&e);

    printf("Numbers generated: %x\n%x\n%x\n",
        e.registers[0],
        e.registers[1],
        e.registers[2]);
}

// dxyn: Draw n-byte sprite at (vx, vy). The sprite address is stored in I.
void test_draw() {
    Emu e = init();
    
    e.memory[512] = 0xa0;
    e.memory[513] = 0x00;
    e.memory[514] = 0x61;
    e.memory[515] = 0x40;
    e.memory[516] = 0x62;
    e.memory[517] = 0x1d;
    e.memory[518] = 0xd1;
    e.memory[519] = 0x25;

    tick(&e);
    tick(&e);
    tick(&e);
    tick(&e);

    assert(e.registers[0xF] == 1);

    e.memory[520] = 0xa0;
    e.memory[521] = 0x05;
    e.memory[522] = 0xd1;
    e.memory[523] = 0x25;

    tick(&e);
    tick(&e);
    assert(e.registers[0xf] == 1);

    e.memory[524] = 0xd1;
    e.memory[525] = 0x25;

    tick(&e);
    tick(&e);
    assert(e.registers[0xf] == 0);

    for (int i = 0; i < DISPLAY_HEIGHT; i++)
        printf("%64lb\n", e.display[i]);
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
    test_sne_regs();
    test_ld_addr();
    test_jp_addr();

    test_rand_byte();
    test_draw();

    printf("All tests were successful.\n");
    return 0;
}