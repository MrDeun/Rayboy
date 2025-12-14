#pragma once

#include "Window.hpp"
#include <stdbool.h>
#include <stdint.h>
struct emu_context{
    bool paused;
    bool running;
    uint64_t ticks;

    uint8_t EmulatorScale = 2;
    bool isHoldRequired = false;
    raylib::Window window;
};

int emu_run(int argc, char** argv);
emu_context* emu_get_context();
