// gamepad.hpp
#pragma once
#include "all.hpp"
#include <atomic>

struct gamepad_state {
    bool START;
    bool SELECT;
    bool A;
    bool B;
    bool UP;
    bool DOWN;
    bool LEFT;
    bool RIGHT;
};

void gamepad_init();
bool gamepad_button_select();
bool gamepad_direction_select();
void gamepad_set_select(uint8_t value);
gamepad_state* gamepad_get_write_buffer();  // Get buffer to write to
void gamepad_swap_buffers();                 // Commit the write
uint8_t gamepad_get_output();