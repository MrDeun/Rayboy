#include "../include/all.hpp"

static constexpr uint32_t target_frame_time = 1000 / 60;
static int32_t prev_frame_time = 0;
static int32_t start_timer = 0;
static int32_t frame_count = 0;

void increment_ly() {
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

void ppu_mode_oam() {
  if (ppu_get_context()->line_ticks >= 80) {
    LCDS_MODE_SET(MODE_XFER);
  }
}
void ppu_mode_xfer() {
  if (ppu_get_context()->line_ticks >= 80 + 172) {
    LCDS_MODE_SET(MODE_HBLANK);
  }
}
void ppu_mode_vblank() {
  if (ppu_get_context()->line_ticks >= TICKS_PER_LINE) {
    increment_ly();

    if (lcd_get_context()->ly >= LINES_PER_FRAMES) {
      LCDS_MODE_SET(MODE_OAM);
      lcd_get_context()->ly = 0;
    }

    ppu_get_context()->line_ticks = 0;
  }
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
