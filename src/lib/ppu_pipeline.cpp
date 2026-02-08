#include "../include/all.hpp"
#include <cstdint>
#include <sys/types.h>
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

uint32_t fetch_sprite_pixels(int bit, uint32_t color, uint8_t bg_color) {
  auto ppu_ctx = ppu_get_context();
  for (int i = 0; i < ppu_ctx->fetch_entry_count; i++) {
    int sp_x = (ppu_ctx->fetched_entries[i].x - 8) +
               ((lcd_get_context()->scroll_x % 8));

    if (sp_x + 8 < ppu_ctx->pfc.fifo_x) {
      // Went past the pixel
      continue;
    }

    int offset = ppu_ctx->pfc.fifo_x - sp_x;
    if (offset < 0 || offset > 7) {
      // Out of bound
      continue;
    }

    bit = offset;
    if (ppu_ctx->fetched_entries[i].f_x_flip) {
      bit = (7 -offset);
    }

    uint8_t high = !!(ppu_ctx->pfc.fetch_entry_data[i * 2] & (1 << bit));
    uint8_t low = !!(ppu_ctx->pfc.fetch_entry_data[i * 2 + 1] & (1 << bit))
                  << 1;

    bool bg_priority = ppu_ctx->fetched_entries[i].f_bgp;
    if (!(high | low)) {
      continue;
    }

    if (!bg_priority || bg_color == 0) {
      color = ppu_ctx->fetched_entries[i].f_pn
                  ? lcd_get_context()->sp2_colors[high | low]
                  : lcd_get_context()->sp1_colors[high | low];
      if (high | low) {
        break;
      }
    }
  }
  return color;
}

bool pipeline_fifo_add() {
  if (ppu_get_context()->pfc.pixel_fifo.size > 8) {
    return false;
  }

  int x =
      ppu_get_context()->pfc.fetch_x - (8 - (lcd_get_context()->scroll_x % 8));
  for (int i = 0; i < 8; i++) {
    int bit = 7 - i;
    uint8_t high = !!(ppu_get_context()->pfc.bgw_fetch_data[1] & (1 << bit));
    uint8_t low = !!(ppu_get_context()->pfc.bgw_fetch_data[2] & (1 << bit))
                  << 1;

    uint32_t color = lcd_get_context()->bg_colors[high | low];

    if (!LCDC_BGW_ENABLE) {
      color = lcd_get_context()->bg_colors[0];
    }

    if (LCDC_OBJ_ENABLE) {
      color = fetch_sprite_pixels(bit, color, high | low);
    }

    if (x >= 0) {
      pixel_fifo_push(color);
      ppu_get_context()->pfc.fifo_x++;
    }
  }

  return true;
}
void pipeline_fifo_reset() {
  auto &fifo = ppu_get_context()->pfc.pixel_fifo;
  while (fifo.size)
    pixel_fifo_pop();
  fifo.head = fifo.tail = nullptr;
  fifo.size = 0;
}

void pipeline_load_sprite_tile() {
  auto ppu_ctx = ppu_get_context();
  auto list_entry = ppu_ctx->line_sprites;
  while (list_entry) {
    int sp_x = (list_entry->entry.x - 8) + (lcd_get_context()->scroll_x % 8);
    if ((sp_x >= ppu_ctx->pfc.fetch_x && sp_x < ppu_ctx->pfc.fetch_x + 8) ||
        (((sp_x + 8) >= ppu_ctx->pfc.fetch_x) &&
         (sp_x + 8) < ppu_ctx->pfc.fetch_x + 8)) {
      // Need to add another entry
      ppu_ctx->fetched_entries[ppu_get_context()->fetch_entry_count++] =
          list_entry->entry;
    }

    list_entry = list_entry->next;
    if (!list_entry || ppu_get_context()->fetch_entry_count >= 3) {
      break;
    }
  }
}

void pipeline_load_sprite_data(uint8_t offset) {
  int cur_y = lcd_get_context()->ly;
  uint8_t sprite_height = LCDC_OBJ_HEIGHT;
  for (int i = 0; i < ppu_get_context()->fetch_entry_count; i++) {
    uint8_t ty = ((cur_y + 16) - ppu_get_context()->fetched_entries[i].y) * 2;

    if (ppu_get_context()->fetched_entries[i].f_y_flip) {
      ty = ((sprite_height * 2) - 2) - ty;
    }

    uint8_t tile_index = ppu_get_context()->fetched_entries[i].tile;
    if (sprite_height == 16) {
      tile_index &= (~1);
    }

    ppu_get_context()->pfc.fetch_entry_data[i * 2 + offset] =
        bus_read(0x8000 + tile_index * 16 + ty + offset);
  }
}

void pipeline_fetch() {
  switch (ppu_get_context()->pfc.cur_fetch_state) {
  case FS_TILE: {
    ppu_get_context()->fetch_entry_count = 0;
    if (LCDC_BGW_ENABLE) {
      ppu_get_context()->pfc.bgw_fetch_data[0] =
          bus_read(LCDC_BG_MAP_AREA + (ppu_get_context()->pfc.map_x / 8) +
                   ((ppu_get_context()->pfc.map_y / 8)) * 32);

      if (LCDC_BGW_DATA_AREA == 0x8800) {
        ppu_get_context()->pfc.bgw_fetch_data[0] += 128;
      }
    }

    if (LCDC_OBJ_ENABLE && ppu_get_context()->line_sprites) {
      pipeline_load_sprite_tile();
    }

    ppu_get_context()->pfc.cur_fetch_state = FS_DATA0;
    ppu_get_context()->pfc.fetch_x += 8;
    break;
  }
  case FS_DATA0: {
    ppu_get_context()->pfc.bgw_fetch_data[1] = bus_read(
        LCDC_BGW_DATA_AREA + (ppu_get_context()->pfc.bgw_fetch_data[0] * 16) +
        ppu_get_context()->pfc.tile_y);
    pipeline_load_sprite_data(0);
    ppu_get_context()->pfc.cur_fetch_state = FS_DATA1;
    break;
  }
  case FS_DATA1: {
    ppu_get_context()->pfc.bgw_fetch_data[2] = bus_read(
        LCDC_BGW_DATA_AREA + (ppu_get_context()->pfc.bgw_fetch_data[0] * 16) +
        ppu_get_context()->pfc.tile_y + 1);
    pipeline_load_sprite_data(1);

    ppu_get_context()->pfc.cur_fetch_state = FS_IDLE;
    break;
  }
  case FS_IDLE: {
    ppu_get_context()->pfc.cur_fetch_state = FS_PUSH;
    break;
  }
  case FS_PUSH: {
    if (pipeline_fifo_add()) {
      ppu_get_context()->pfc.cur_fetch_state = FS_TILE;
    }
    break;
  }
  }
}
void pipeline_push_pixel() {
  if (ppu_get_context()->pfc.pixel_fifo.size >= 8) {
    uint32_t pixel_data = pixel_fifo_pop();
    if (ppu_get_context()->pfc.line_x >= (lcd_get_context()->scroll_x % 8)) {
      ppu_get_context()->video_buffer[ppu_get_context()->pfc.pushed_x +
                                      (lcd_get_context()->ly * XRES)] =
          pixel_data;
      ppu_get_context()->pfc.pushed_x++;
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