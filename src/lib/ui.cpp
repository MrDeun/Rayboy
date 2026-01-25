#include "../include/all.hpp"

#include "fmt/core.h"
#include "raylib.h"
#include <atomic>
#include <cstdint>
#include <cstring>


void RayboyUI::drawScreen(){
  DrawRectangle(
      screen_x - 4,
      screen_y - 4,
      GB_W * SCALE + 8,
      GB_H * SCALE + 8,
      PANEL_COLOR);

  DrawRectangleLines(
      screen_x - 4,
      screen_y - 4,
      GB_W * SCALE + 8,
      GB_H * SCALE + 8,
      ACCENT_COLOR);

  // Draw LCD
  DrawTextureEx(
      screen_texture,
      {(float)screen_x, (float)screen_y},
      0.0f,
      SCALE,
      WHITE);
}

void RayboyUI::updateScreenTexture(){
  if(!shared->frame_ready.load(std::memory_order_acquire)) return;

  int r = shared->read_index.load(std::memory_order_acquire);
  const auto src = shared->frames[r];
  std::memcpy(screen_image.data,src,GB_H*GB_W*sizeof(uint32_t));

  UpdateTexture(screen_texture, screen_image.data);
  shared->frame_ready.store(false,std::memory_order_release);
}

void RayboyUI::setup(EmulatorShared *shared_ptr) {
  SetTraceLogLevel(LOG_NONE);
  shared = shared_ptr;

  // Calculate dimensions
  int tile_viewer_width = TILES_PER_ROW * TILE_SIZE * SCALE;
  int tile_viewer_height = TILES_PER_COL * TILE_SIZE * SCALE;

  window_width = PANEL_WIDTH + UI_PADDING * 3 + tile_viewer_width;
  window_height = tile_viewer_height + UI_PADDING * 2 + 30; // 30 for status bar

  // Layout positions
  stats_panel_x = UI_PADDING;
  stats_panel_y = UI_PADDING;
  tile_viewer_x = PANEL_WIDTH + UI_PADDING * 2;
  tile_viewer_y = UI_PADDING;

  InitWindow(window_width, window_height, "Rayboy - GameBoy Emulator");
  SetTargetFPS(60);

  // Load default font
  ui_font = GetFontDefault();

  // Setup tile viewer image
  tile_image = GenImageColor(tile_viewer_width, tile_viewer_height,
                             (Color){17, 17, 17, 255});
  tile_texture = LoadTextureFromImage(tile_image);
  screen_x = stats_panel_x + 10;
  screen_y = stats_panel_y + 40;

  // Create screen image & texture
  screen_image = GenImageColor(GB_W, GB_H, BLACK);
  screen_texture = LoadTextureFromImage(screen_image);
}

void RayboyUI::shutdown() {
  UnloadTexture(tile_texture);
  UnloadImage(tile_image);
  UnloadTexture(screen_texture);
  UnloadImage(screen_image);
  CloseWindow();
}

void RayboyUI::handleInput() {
  if (IsKeyPressed(KEY_SPACE)) {
    bool current = shared->paused.load(std::memory_order_acquire);
    shared->paused.store(!current, std::memory_order_release);
  }

  if (IsKeyPressed(KEY_ESCAPE)) {
    should_exit_flag = true;
    shared->should_exit.store(true, std::memory_order_release);
  }

  if (IsKeyPressed(KEY_F1))
    show_tile_viewer = !show_tile_viewer;
  if (IsKeyPressed(KEY_F2))
    show_stats = !show_stats;
  if (IsKeyPressed(KEY_H))
    show_help = !show_help;
}

void RayboyUI::renderTile(const uint8_t *tile_data, int tile_num, int x,
                          int y) {
  uint16_t tile_offset = tile_num * 16;

  if (tile_offset + 15 >= 0x1800) {
    return;
  }

  for (int tile_y = 0; tile_y < 8; tile_y++) {
    uint16_t byte_offset = tile_offset + (tile_y * 2);
    uint8_t byte1 = tile_data[byte_offset];
    uint8_t byte2 = tile_data[byte_offset + 1];

    for (int tile_x = 0; tile_x < 8; tile_x++) {
      int bit = 7 - tile_x;
      uint8_t color_id = ((byte2 >> bit) & 1) << 1 | ((byte1 >> bit) & 1);

      int px = x + (tile_x * SCALE);
      int py = y + (tile_y * SCALE);

      Color color = TILE_COLORS[color_id];
      for (int sy = 0; sy < SCALE; sy++) {
        for (int sx = 0; sx < SCALE; sx++) {
          ImageDrawPixel(&tile_image, px + sx, py + sy, color);
        }
      }
    }
  }
}

void RayboyUI::updateTileImage() {
  if (!shared->swapTileBuffers()) {
    return;
  }

  const std::vector<uint8_t> &tile_buffer = shared->getTileBuffer();
  const uint8_t *tile_data = tile_buffer.data();

  ImageClearBackground(&tile_image, (Color){17, 17, 17, 255});

  for (int row = 0; row < TILES_PER_COL; row++) {
    for (int col = 0; col < TILES_PER_ROW; col++) {
      int tile_num = row * TILES_PER_ROW + col;
      int x = col * TILE_SIZE * SCALE;
      int y = row * TILE_SIZE * SCALE;
      renderTile(tile_data, tile_num, x, y);
    }
  }

  texture_needs_update = true;
}

void RayboyUI::drawPanel(int x, int y, int width, int height,
                         const char *title) {
  // Panel background
  DrawRectangle(x, y, width, height, PANEL_COLOR);
  DrawRectangleLines(x, y, width, height, ACCENT_COLOR);

  // Title bar
  DrawRectangle(x, y, width, 30, ACCENT_COLOR);
  DrawText(title, x + 10, y + 8, 14, WHITE);
}

void RayboyUI::drawButton(int x, int y, int width, int height, const char *text,
                          bool *state) {
  Rectangle rect = {(float)x, (float)y, (float)width, (float)height};
  bool hovered = CheckCollisionPointRec(GetMousePosition(), rect);

  Color bg = *state ? ACCENT_COLOR : PANEL_COLOR;
  if (hovered && !*state) {
    bg = (Color){50, 50, 55, 255};
  }

  DrawRectangleRec(rect, bg);
  DrawRectangleLinesEx(rect, 1, ACCENT_COLOR);

  int text_width = MeasureText(text, 10);
  DrawText(text, x + (width - text_width) / 2, y + (height - 10) / 2, 10,
           TEXT_COLOR);

  if (hovered && IsMouseButtonPressed(MOUSE_LEFT_BUTTON)) {
    *state = !*state;
  }
}

void RayboyUI::drawStatsPanel() {
  if (!show_stats)
    return;

  int panel_height = 300;
  drawPanel(stats_panel_x, stats_panel_y, PANEL_WIDTH, panel_height,
            "Statistics");

  int text_y = stats_panel_y + 40;
  int line_height = 20;

  // Get stats
  uint64_t frames = shared->frame_count.load(std::memory_order_relaxed);
  uint32_t fps = shared->fps.load(std::memory_order_relaxed);
  bool paused = shared->paused.load(std::memory_order_relaxed);
  bool running = shared->running.load(std::memory_order_relaxed);

  // Display stats
  std::string buffer;
  buffer.reserve(256);
  buffer = fmt::format("Frame: {}", frames);
  DrawText(buffer.c_str(), stats_panel_x + 10, text_y, 12, TEXT_COLOR);
  text_y += line_height;

  buffer = fmt::format("FPS {}", fps);
  DrawText(buffer.c_str(), stats_panel_x + 10, text_y, 12, TEXT_COLOR);
  text_y += line_height;

  buffer = fmt::format("Status: {}", paused    ? "PAUSED"
                                     : running ? "RUNNING"
                                               : "STOPPED");
  Color status_color = paused ? YELLOW : running ? GREEN : RED;
  DrawText("Status:", stats_panel_x + 10, text_y, 12, TEXT_COLOR);
  DrawText(paused    ? "PAUSED"
           : running ? "RUNNING"
                     : "STOPPED",
           stats_panel_x + 70, text_y, 12, status_color);
  text_y += line_height * 2;

  // Separator
  DrawLine(stats_panel_x + 10, text_y - 10, stats_panel_x + PANEL_WIDTH - 10,
           text_y - 10, ACCENT_COLOR);

  // Controls section
  DrawText("Controls:", stats_panel_x + 10, text_y, 12, ACCENT_COLOR);
  text_y += line_height + 5;

  const char *controls[] = {"SPACE - Pause/Resume", "F1 - Toggle Tiles",
                            "F2 - Toggle Stats", "H - Toggle Help",
                            "ESC - Exit"};

  for (int i = 0; i < 5; i++) {
    DrawText(controls[i], stats_panel_x + 15, text_y, 10, TEXT_COLOR);
    text_y += line_height;
  }
}

void RayboyUI::drawTileViewer() {
  if (!show_tile_viewer)
    return;

  // Draw border
  DrawRectangleLines(tile_viewer_x - 2, tile_viewer_y - 2,
                     tile_texture.width + 4, tile_texture.height + 4,
                     ACCENT_COLOR);

  // Draw title
  DrawText("Tile Viewer (0x8000-0x97FF)", tile_viewer_x, tile_viewer_y - 20, 14,
           TEXT_COLOR);

  // Draw texture
  if (tile_texture.id > 0) {
    DrawTexture(tile_texture, tile_viewer_x, tile_viewer_y, WHITE);
  }

  // Draw grid overlay (optional)
  Color grid_color = {60, 60, 70, 100};
  for (int i = 0; i <= TILES_PER_ROW; i++) {
    int x = tile_viewer_x + i * TILE_SIZE * SCALE;
    DrawLine(x, tile_viewer_y, x, tile_viewer_y + tile_texture.height,
             grid_color);
  }
  for (int i = 0; i <= TILES_PER_COL; i++) {
    int y = tile_viewer_y + i * TILE_SIZE * SCALE;
    DrawLine(tile_viewer_x, y, tile_viewer_x + tile_texture.width, y,
             grid_color);
  }
}

void RayboyUI::drawStatusBar() {
  int bar_y = window_height - 25;
  DrawRectangle(0, bar_y, window_width, 25, PANEL_COLOR);
  DrawLine(0, bar_y, window_width, bar_y, ACCENT_COLOR);

  bool paused = shared->paused.load(std::memory_order_relaxed);
  const char *status_text =
      paused ? "PAUSED - Press SPACE to resume" : "RUNNING - Press H for help";
  DrawText(status_text, 10, bar_y + 6, 12, TEXT_COLOR);
}

void RayboyUI::drawHelpOverlay() {
  if (!show_help)
    return;

  // Semi-transparent background
  DrawRectangle(0, 0, window_width, window_height, (Color){0, 0, 0, 200});

  // Help panel
  int help_width = 400;
  int help_height = 300;
  int help_x = (window_width - help_width) / 2;
  int help_y = (window_height - help_height) / 2;

  drawPanel(help_x, help_y, help_width, help_height, "Help");

  int text_y = help_y + 50;
  int line_height = 25;

  const char *help_text[] = {"Keyboard Controls:",
                             "",
                             "SPACE     - Pause/Resume emulation",
                             "F1        - Toggle tile viewer",
                             "F2        - Toggle statistics panel",
                             "H         - Toggle this help screen",
                             "ESC       - Exit emulator",
                             "",
                             "Press H to close this help screen"};

  for (int i = 0; i < 9; i++) {
    int font_size = (i == 0 || i == 8) ? 14 : 12;
    Color color = (i == 0) ? ACCENT_COLOR : TEXT_COLOR;
    DrawText(help_text[i], help_x + 20, text_y, font_size, color);
    text_y += line_height;
  }
}

void RayboyUI::update() {
  handleInput();
  updateScreenTexture();
  updateTileImage();

  if (texture_needs_update) {
    UpdateTexture(tile_texture, tile_image.data);
    texture_needs_update = false;
  }
}

void RayboyUI::draw() {
  BeginDrawing();
  ClearBackground(BG_COLOR);

  // Draw main UI components
  drawScreen();
  drawStatsPanel();
  drawTileViewer();
  drawStatusBar();

  // Draw overlay last
  drawHelpOverlay();

  EndDrawing();
}