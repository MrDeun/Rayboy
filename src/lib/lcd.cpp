#include "../include/all.hpp"
#include "raylib.h"
#include <cstdint>

static lcd_context ctx = {};

static uint32_t colors_defaults[4];
lcd_context *lcd_get_context() { return &ctx; }
void lcd_init() {
  ctx.lcdc = 0;
  ctx.scroll_x = 0;
  ctx.scroll_y = 0;
  ctx.ly = 0;
  ctx.ly_compare = 0;
  ctx.bg_palette = 0xFC;
  ctx.obj_palette[0] = 0xFF;
  ctx.obj_palette[1] = 0xFF;

  ctx.win_y = 0;
  ctx.win_x = 0;
  colors_defaults[0] = 0xFFFFFFFF; // White
  colors_defaults[1] = 0xFFAAAAAA; // Light gray
  colors_defaults[2] = 0xFF555555; // Dark gray  
  colors_defaults[3] = 0xFF000000; // Black
  
  for (int i = 0; i < 4; i++) {

    ctx.bg_colors[i] = colors_defaults[i];
    ctx.sp1_colors[i] = colors_defaults[i];
    ctx.sp2_colors[i] = colors_defaults[i];
  }
}

void update_pallete(uint8_t value, uint8_t pal) {
  uint32_t *p_colors;

  switch (pal) {
  case 0:
    p_colors = ctx.bg_colors;
    break;
  case 1:
    p_colors = ctx.sp1_colors;
    break;
  case 2:
    p_colors = ctx.sp2_colors;
    break;
  default:
    ERROR("Illegal pallete value");
  }

  p_colors[0] = colors_defaults[(value >> 0) & 0b11];
  p_colors[1] = colors_defaults[(value >> 2) & 0b11];
  p_colors[2] = colors_defaults[(value >> 4) & 0b11];
  p_colors[3] = colors_defaults[(value >> 6) & 0b11];
}

uint8_t lcd_read(uint16_t address) {
  uint8_t offset = address - 0xff40;
  auto ptr = (uint8_t *)lcd_get_context();

  return ptr[offset];
}

void lcd_write(uint16_t address, uint8_t value) {
  uint8_t offset = (address - 0xff40);
  uint8_t *ptr = (uint8_t *)lcd_get_context();

  if (offset == 6) {
    dma_start(value);
  }

  switch (address) {
  case 0xff47:
    update_pallete(value, 0);
    break;
  case 0xff48:
    update_pallete(value, 1);
    break;
  case 0xff49:
    update_pallete(value, 2);
    break;
  }

  ptr[offset] = value;
}
