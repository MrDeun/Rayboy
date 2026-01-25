#pragma once

#include "all.hpp"
#include "raylib.h"
#include <array>
#include <cstdint>
#include <sys/types.h>

static const std::array<Color, 4> tileColors{WHITE, DARKGRAY, LIGHTGRAY, BLACK};

class RayboyUI {
public:
  RayboyUI() = default;
  ~RayboyUI() = default;

  void setup(EmulatorShared *shared);
  void shutdown();
  void update();
  void draw();
  bool shouldExit() const { return should_exit_flag; }
  static constexpr Color TILE_COLORS[4] = {
      {255, 255, 255, 255}, // White
      {170, 170, 170, 255}, // Light gray
      {85, 85, 85, 255},    // Dark gray
      {0, 0, 0, 255}        // Black
  };

private:
  EmulatorShared *shared = nullptr;
  bool should_exit_flag = false;

  // Display constants
  static constexpr int SCALE = 3;
  static constexpr int TILE_SIZE = 8;
  static constexpr int TILES_PER_ROW = 16;
  static constexpr int TILES_PER_COL = 24;
  static constexpr int UI_PADDING = 20;
  static constexpr int PANEL_WIDTH = 250;

  static constexpr int GB_W = 160;
  static constexpr int GB_H = 144;
  Image screen_image{};
  Texture2D screen_texture{};
  int screen_x = 0;
  int screen_y = 0;

  // Colors
  static constexpr Color BG_COLOR = {25, 25, 30, 255};
  static constexpr Color PANEL_COLOR = {35, 35, 40, 255};
  static constexpr Color ACCENT_COLOR = {100, 150, 255, 255};
  static constexpr Color TEXT_COLOR = {220, 220, 220, 255};

  // Rendering resources
  Image tile_image;
  Texture2D tile_texture;
  bool texture_needs_update = false;
  Font ui_font;

  // Layout positions
  int window_width;
  int window_height;
  int tile_viewer_x;
  int tile_viewer_y;
  int stats_panel_x;
  int stats_panel_y;

  // UI state
  bool show_tile_viewer = true;
  bool show_stats = true;
  bool show_help = false;

  // Private methods
  void updateScreenTexture();
  void drawScreen();


  void handleInput();
  void updateTileImage();
  void renderTile(const uint8_t *tile_data, int tile_num, int x, int y);
  void drawStatsPanel();
  void drawTileViewer();
  void drawHelpOverlay();
  void drawStatusBar();

  void drawPanel(int x, int y, int width, int height, const char *title);
  void drawButton(int x, int y, int width, int height, const char *text,
                  bool *state);
};
