#include <stdio.h>
#include <assert.h>
#include "include/core.h"

int main(int argc, char *argv[]) {
    if (argc < 2) {
        fprintf(stderr, "No file name given.\nUsage: %s <filename>\n", argv[0]);
    }

    FILE *rom = fopen(argv[1], "r");

    if (rom == NULL) {
        fprintf(stderr, "Error: could not open file.\n");
        return 1;
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
        return 1;
    }

    return 0;
}