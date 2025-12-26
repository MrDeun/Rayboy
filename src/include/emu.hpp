#pragma once

#include <stdbool.h>
#include <stdint.h>
struct emu_context{
    bool paused;
    bool running;
    bool die;
    uint64_t ticks;

    uint8_t EmulatorScale = 1;
};

int emu_run(int argc, char** argv);
emu_context* emu_get_context();