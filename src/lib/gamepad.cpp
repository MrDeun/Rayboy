// gamepad.cpp
#include "../include/all.hpp"
#include <cstdint>
#include <atomic>

struct gamepad_context {
    bool button_select;
    bool direction_select;
    gamepad_state buffers[2];
    std::atomic<int> read_index;  // Use int instead of pointer
    int write_index;
};

static gamepad_context ctx;

void gamepad_init() {
    ctx.button_select = false;
    ctx.direction_select = false;
    ctx.buffers[0] = {0};
    ctx.buffers[1] = {0};
    ctx.read_index.store(0, std::memory_order_release);
    ctx.write_index = 1;
}

bool gamepad_button_select() { return ctx.button_select; }
bool gamepad_direction_select() { return ctx.direction_select; }

void gamepad_set_select(uint8_t value) {
    ctx.button_select = value & 0x20;
    ctx.direction_select = value & 0x10;  // Fixed: different bit
}

// Raylib thread: get buffer to write input into
gamepad_state* gamepad_get_write_buffer() {
    return &ctx.buffers[ctx.write_index];
}

// Raylib thread: commit the changes
void gamepad_swap_buffers() {
    int old_read = ctx.read_index.load(std::memory_order_acquire);
    int new_read = ctx.write_index;
    ctx.write_index = old_read;
    ctx.read_index.store(new_read, std::memory_order_release);
}

// Emulator thread: read current state
uint8_t gamepad_get_output() {
    int idx = ctx.read_index.load(std::memory_order_acquire);
    gamepad_state* state = &ctx.buffers[idx];
    
    uint8_t res = 0xCF;
    
    if (!gamepad_button_select()) {
        if (state->START) res &= ~(1 << 3);
        else if (state->SELECT) res &= ~(1 << 2);
        else if (state->B) res &= ~(1 << 1);
        else if (state->A) res &= ~(1 << 0);
    }
    
    if (!gamepad_direction_select()) {
        if (state->DOWN) res &= ~(1 << 3);
        else if (state->UP) res &= ~(1 << 2);
        else if (state->LEFT) res &= ~(1 << 1);
        else if (state->RIGHT) res &= ~(1 << 0);
    }
    
    return res;
}