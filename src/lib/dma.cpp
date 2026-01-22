#pragma once

#include "../include/all.hpp"

#include <cstdint>

struct dma_context {
  bool active;
  uint8_t byte;
  uint8_t value;
  uint8_t delay;
};

static dma_context ctx = {0};

extern EmulatorShared* g_shared;

void dma_start(uint8_t start) {
  ctx.active = true;
  ctx.byte = 0;
  ctx.delay = 2;
  ctx.value = start;
}
void dma_tick() {
    if (!ctx.active) {
        return;
    }

    if (ctx.delay) {
        ctx.delay--;
        return;  // Add return here - was missing
    }

    // Check if tile copying is in progress
    // If so, pause DMA briefly to let it finish
    if (g_shared && g_shared->isCopyingTiles()) {
        // Tile copying takes ~6000 reads, DMA only does 160
        // So just wait one tick - copying will finish very soon
        return;
    }

    ppu_oam_write(ctx.byte, bus_read((ctx.value * 0x100) + ctx.byte));
    ctx.byte++;
    ctx.active = ctx.byte < 0xA0;
}

bool dma_transferring() {
    return ctx.active;
}
