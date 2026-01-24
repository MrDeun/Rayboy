#include "../include/all.hpp"
void pixel_fifo_push(uint32_t value) {
  fifo_entry *next = new fifo_entry;
  auto ppu_ctx = ppu_get_context();
  next->next = nullptr;
  next->value = value;
  if (!ppu_ctx->pfc.pixel_fifo.head) {
    ppu_ctx->pfc.pixel_fifo.head = ppu_ctx->pfc.pixel_fifo.tail = next;
  } else {
    ppu_ctx->pfc.pixel_fifo.tail->next = next;
    ppu_ctx->pfc.pixel_fifo.tail = next;
  }

  ppu_ctx->pfc.pixel_fifo.size++;
}

uint32_t pixel_fifo_pop() {
  auto ppu_ctx = ppu_get_context();
  if (ppu_ctx->pfc.pixel_fifo.size <= 0) {
    ERROR("Invalid FIFO operation -> Tried to pop, but Pixel FIFO is empty");
  }
  auto popped = ppu_ctx->pfc.pixel_fifo.head;
  ppu_ctx->pfc.pixel_fifo.head = popped->next;
  ppu_ctx->pfc.pixel_fifo.size--;
  uint32_t value = popped->value;
  delete popped;

  return value;
}

void pipeline_fetch() {
  switch (ppu_get_context()->pfc.cur_fetch_state) {
  case FS_TILE: {
    if(LCDC_BGW_ENABLE){
        ppu_get_context()->pfc.bgw_fetch_data[0] = bus_read(LCDC_BG_MAP_AREA + (ppu_get_context()->pfc.map_x/8));
    }
    break;
  }
  case FS_DATA0: {
    break;
  }
  case FS_DATA1: {
    break;
  }
  case FS_IDLE: {
    break;
  }
  case FS_PUSH: {
    break;
  }
  }
}
void pipeline_push_pixel() {
  if (ppu_get_context()->pfc.pixel_fifo.size > 8) {
    uint32_t pixel_data = pixel_fifo_pop();
    if (ppu_get_context()->pfc.line_x >= (lcd_get_context()->scroll_x % 8)) {
      ppu_get_context()->video_buffer[ppu_get_context()->pfc.pushed_x +
                                      (lcd_get_context()->ly * XRES)] =
          pixel_data;
      ppu_get_context()->pfc.pushed_x;
    }

    ppu_get_context()->pfc.line_x++;
  }
}
void pipeline_process() {
  ppu_get_context()->pfc.map_y =
      (lcd_get_context()->ly + lcd_get_context()->scroll_y);
  ppu_get_context()->pfc.map_x =
      (ppu_get_context()->pfc.fetch_x + lcd_get_context()->scroll_x);

  ppu_get_context()->pfc.tile_y =
      ((lcd_get_context()->ly + lcd_get_context()->scroll_y) % 8) * 2;
  if (!(ppu_get_context()->line_ticks & 1)) {
    pipeline_fetch();
  }

  pipeline_push_pixel();
}