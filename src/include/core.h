#include <stdint.h>
#include <stdbool.h>

#define NUM_REGISTERS                   16
#define MEMORY_SIZE                   4096
#define PROGRAM_START                0x200
#define STACK_SIZE                      16

#define DISPLAY_WIDTH                   64
#define DISPLAY_HEIGHT                  32

#define MASK            0x8000000000000000

typedef struct {
    uint8_t registers[NUM_REGISTERS];
    uint16_t pc;
    uint16_t i_reg;
    uint8_t st;
    uint8_t dt;
    uint8_t sp;
    uint16_t stack[STACK_SIZE];
    uint8_t memory[MEMORY_SIZE];
    uint64_t display[DISPLAY_HEIGHT];
    // bool display[DISPLAY_HEIGHT * DISPLAY_WIDTH];
} Emu;

Emu init();

uint16_t pop(Emu *e);
void push(Emu *e, uint16_t return_address);
void tick(Emu *e);
uint16_t fetch(Emu *e);