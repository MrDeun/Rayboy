#pragma once

#include "Window.hpp"
#include <mutex>
#include <stdbool.h>
#include <stdint.h>
struct emu_context{
    bool paused;
    bool running;
    bool die;
    uint64_t ticks;
    
};
struct EmulatorShared {
  std::mutex *mutex;
  emu_context *emu_ctx;
};
int emu_run(int argc, char** argv);
emu_context* emu_get_context();
