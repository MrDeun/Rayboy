#include "../include/emu.h"
#include "../include/raylib.h"
#include "../include/cart.h"
#include <stdio.h>
static emu_context ctx;
emu_context *emu_get_context() {
    return &ctx;
}

void delay(uint32_t ms){
    WaitTime(ms/1000.0);
}

int emu_run(int argc, char **argv) {
    if (argc < 2){
        printf("Usage: emu <rom_file>\n");
        return -1;
    }
    if (!cart_load(argv[1])){
        printf("Failed to load a rom file: %s\n",argv[1]);
        return -2;
    }

    printf("Card Loaded.. \n");
    
}