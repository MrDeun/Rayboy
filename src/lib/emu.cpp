#include "../include/emu.hpp"
#include "../include/cart.hpp"
#include <cstdio>
static emu_context ctx;
emu_context *emu_get_context() {
    return &ctx;
}
void delay(uint32_t ms);

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