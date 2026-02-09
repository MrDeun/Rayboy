#include "../include/all.hpp"
#include <cstdint>
#include <cstring>

static constexpr uint32_t target_frame_time = 1000 / 60;
static int32_t prev_frame_time = 0;
static int32_t start_timer = 0;
static int32_t frame_count = 0;

bool window_visible();

void increment_ly() {
  if (window_visible() && lcd_get_context()->ly >= lcd_get_context()->win_y &&
      lcd_get_context()->ly < lcd_get_context()->win_y + YRES) {
    ppu_get_context()->window_line++;
  }
  lcd_get_context()->ly++;
  if (lcd_get_context()->ly == lcd_get_context()->ly_compare) {
    LCDS_LYC_SET(1);
    if (LCDS_STAT_INT(SS_LYC)) {
      cpu_request_interupts(IT_LCD_STAT);
    }
  } else {
    LCDS_LYC_SET(0);
  }
}
void ppu_publish_stats(EmulatorShared *shared) {
  int w = shared->ppu_stats_write_index.load(std::memory_order_relaxed);
  int r = shared->ppu_stats_read_index.load(std::memory_order_relaxed);

  // Copy local → shared
  shared->ppu_stats[w] = ppu_get_context()->local_stats;

  // Swap
  shared->ppu_stats_read_index.store(w, std::memory_order_release);
  shared->ppu_stats_write_index.store(r, std::memory_order_relaxed);

  shared->ppu_stats_ready.store(true, std::memory_order_release);
}

void load_line_sprites() {
  auto ppu_ctx = ppu_get_context();

  int cur_y = lcd_get_context()->ly;
  uint8_t sprite_height = LCDC_OBJ_HEIGHT;
  std::memset(ppu_ctx->line_entry_array, 0, sizeof(ppu_ctx->line_entry_array));
  for (int i = 0; i < 40; i++) {
    oam_entry e = ppu_ctx->oam_ram[i];
    if (!e.x) {
      // x = 0 means its not visible - skip it
      continue;
    }
    if (ppu_ctx->line_sprite_count >= 10) {
      break; // only 10 sprites per line
    }
    if (e.y <= cur_y + 16 && e.y + sprite_height > cur_y + 16) {
      _oam_line_entry *entry =
          &ppu_ctx->line_entry_array[ppu_ctx->line_sprite_count++];
      entry->entry = e;
      entry->next = NULL;

      if (!ppu_ctx->line_sprites || ppu_ctx->line_sprites->entry.x > e.x) {
        entry->next = ppu_ctx->line_sprites;
        ppu_ctx->line_sprites = entry;
        continue;
      }

      _oam_line_entry *entry_list = ppu_ctx->line_sprites;
      _oam_line_entry *prev = entry_list;

      while (entry_list) {
        if (entry_list->entry.x > e.x) {
          prev->next = entry;
          entry->next = entry_list;
          break;
        }
        if (!entry_list->next) {
          entry_list->next = entry;
          break;
        }
        prev = entry_list;
        entry_list = entry_list->next;
      }
    }
  }
}

void ppu_mode_oam() {
  if (ppu_get_context()->line_ticks >= 80) {
    LCDS_MODE_SET(MODE_XFER);
    ppu_get_context()->pfc.cur_fetch_state = FS_TILE;
    ppu_get_context()->pfc.line_x = 0;
    ppu_get_context()->pfc.fetch_x = 0;
    ppu_get_context()->pfc.pushed_x = 0;
    ppu_get_context()->pfc.fifo_x = 0;
  }
  if (ppu_get_context()->line_ticks == 1) {
    ppu_get_context()->line_sprites = 0;
    ppu_get_context()->line_sprite_count = 0;

    load_line_sprites();
  }
}
void ppu_mode_xfer() {
  pipeline_process();
  if (ppu_get_context()->pfc.pushed_x >= XRES) {
    pipeline_fifo_reset();
    LCDS_MODE_SET(MODE_HBLANK);
    if (LCDS_STAT_INT(SS_HBLANK)) {
      cpu_request_interupts(IT_LCD_STAT);
    }
  }
}
void ppu_mode_vblank() {
  auto *shared = get_shared_emulator_state();
  auto *ctx = ppu_get_context();
  if (ppu_get_context()->line_ticks >= TICKS_PER_LINE) {
    increment_ly();

    if (lcd_get_context()->ly >= LINES_PER_FRAMES) {
      int write_idx =
          shared->ppu_stats_write_index.load(std::memory_order_relaxed);
      shared->ppu_stats[write_idx] = ctx->local_stats;

      // 3. Signal to UI thread
      shared->ppu_stats_read_index.store(write_idx, std::memory_order_release);
      shared->ppu_stats_write_index.store(1 - write_idx,
                                          std::memory_order_relaxed);
      shared->ppu_stats_ready.store(true, std::memory_order_release);

      // 4. RESET local stats for the next frame
      memset(&ctx->local_stats, 0, sizeof(PPUStats));

      LCDS_MODE_SET(MODE_OAM);
      lcd_get_context()->ly = 0;
      ppu_get_context()->window_line = 0;
    }

    ppu_get_context()->line_ticks = 0;
  }
}
void ppu_present_frame(EmulatorShared *shared) {
  int w = shared->write_index.load(std::memory_order_relaxed);
  int r = shared->read_index.load(std::memory_order_relaxed);

  // Swap
  shared->read_index.store(w, std::memory_order_release);
  shared->write_index.store(r, std::memory_order_relaxed);

  // Point PPU to NEW write buffer
  ppu_get_context()->video_buffer = shared->frames[r];

  shared->frame_ready.store(true, std::memory_order_release);
}

void ppu_mode_hblank() {
  if (ppu_get_context()->line_ticks >= TICKS_PER_LINE) {
    increment_ly();
    if (lcd_get_context()->ly >= YRES) {
      LCDS_MODE_SET(MODE_VBLANK);
      cpu_request_interupts(IT_VBLANK);

      if (LCDS_STAT_INT(SS_VBLANK)) {
        cpu_request_interupts(IT_LCD_STAT);
      }
      ppu_present_frame(get_shared_emulator_state());
      ppu_publish_stats(get_shared_emulator_state());
      ppu_get_context()->current_frame++;

      uint32_t end = get_ticks();
      uint32_t frame_time = end - prev_frame_time;

      if (frame_time < target_frame_time) {
        delay(target_frame_time - frame_time);
      }

      if (end - start_timer >= 1000) {
        uint32_t fps = frame_count;
        start_timer = end;
        frame_count = 0;
      }

      frame_count++;
      prev_frame_time = get_ticks();
    } else {
      LCDS_MODE_SET(MODE_OAM);
    }

    ppu_get_context()->line_ticks = 0;
  }
}
