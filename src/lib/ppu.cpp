#include "../include/all.hpp"
#include <atomic>
#include <cstddef>

static ppu_context ctx = {0};
ppu_context *ppu_get_context() { return &ctx; }
void ppu_tick() {
  ctx.line_ticks++;
  switch (LCDS_MODE) {
  case MODE_OAM:
    ppu_mode_oam();
    break;
  case MODE_XFER:
    ppu_mode_xfer();
    break;
  case MODE_VBLANK:
    ppu_mode_vblank();
    break;
  case MODE_HBLANK:
    ppu_mode_hblank();
    break;
  }
  auto *shared = get_shared_emulator_state();
  static int tile_update_timer = 0;
  if (++tile_update_timer >= 60) {
    tile_update_timer = 0;
    if (!shared->isCopyingTiles()) {
      std::vector<uint8_t> &dest = shared->getCpuTileBuffer();
      std::memcpy(dest.data(), ctx.vram, 0x1800);
      shared->signalTileDataReady();
    }
  }
}
void ppu_init() {
  auto *shared = get_shared_emulator_state();
  ctx.local_stats = {};
  ctx.current_frame = 0;
  ctx.line_ticks = 0;
  int w = shared->write_index.load(std::memory_order_relaxed);
  ctx.video_buffer = shared->frames[w];

  ctx.pfc.line_x = 0;
  ctx.pfc.pushed_x = 0;
  ctx.pfc.fetch_x = 0;
  ctx.pfc.pixel_fifo.size = 0;
  ctx.pfc.pixel_fifo.head = ctx.pfc.pixel_fifo.tail = nullptr;
  ctx.pfc.cur_fetch_state = FS_TILE;

  ctx.line_sprites = 0;
  ctx.fetch_entry_count = 0;

  lcd_init();
  LCDS_MODE_SET(MODE_OAM);
  memset(ctx.oam_ram, 0, sizeof(ctx.oam_ram));
  memset(ctx.video_buffer, 0, YRES * XRES * sizeof(uint32_t));
}

void ppu_oam_write(uint16_t address, uint8_t value) {
  if (address >= 0xfe00) {
    address -= 0xfe00;
  }
  uint8_t *oam_ptr = (uint8_t *)ctx.oam_ram;
  oam_ptr[address] = value;
}
uint8_t ppu_oam_read(uint16_t address) {
  if (address >= 0xfe00) {
    address -= 0xfe00;
  }
  uint8_t *oam_ptr = (uint8_t *)ctx.oam_ram;
  return oam_ptr[address];
}

void ppu_vram_write(uint16_t address, uint8_t value) {
  ctx.vram[address - 0x8000] = value;
}
uint8_t ppu_vram_read(uint16_t address) { return ctx.vram[address - 0x8000]; }
