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
  }

  ppu_oam_write(ctx.byte, bus_read((ctx.value * 0x100) + ctx.byte));
  ctx.byte++;
  ctx.active = ctx.byte < 0xA0;
  if (!ctx.active) {
    fmt::println("DMA Stopped");
  }
}

bool dma_transferring() {
    return ctx.active;
}