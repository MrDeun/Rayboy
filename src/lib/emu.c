#include "../include/emu.h"
#include "../include/raylib.h"
static emu_context ctx;
emu_context *emu_get_context() {
    return &ctx;
}

void delay(uint32_t ms){
    WaitTime(ms/1000.0);
}