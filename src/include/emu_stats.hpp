#pragma once
#include "all.hpp"

struct CPUStats {
    uint64_t cycles;
    uint64_t instructions;
    uint64_t time_us;
};

struct PPUStats {
    uint32_t pixels_pushed;
    uint32_t max_fifo_size;

    uint32_t oam_ticks;
    uint32_t xfer_ticks;
    uint32_t hblank_ticks;
    uint32_t vblank_ticks;

    uint32_t frame_time_ms;
    uint32_t fps;
    uint64_t frame_number;
};