#pragma once
#include "../include/all.hpp"

#include "Window.hpp"
#include "common.hpp"
#include "emu.hpp"
#include "raylib.h"
#include <array>
#include <cstdint>
#include <sys/types.h>
#include <vector>

static const std::array<Color, 4> tileColors{WHITE, DARKGRAY, LIGHTGRAY, BLACK};

class RayboyUI {
private:
  std::vector<uint8_t> tile_data_copy; // Local copy for rendering
  std::mutex render_mutex;             // Separate mutex for render data
  Image debugImage;
  Texture2D debugTexture;

  static constexpr uint16_t _ScreenWidth = 1024;
  static constexpr uint16_t _ScreenHeight = 768;
  static constexpr uint8_t scale = 4;

public:
  void shutdown();
  void displayTile(uint16_t startLocation, uint16_t tileNum, int x, int y);
  void setup();
  void draw();
  void displayTileFromCopy(uint16_t tileNum, int x, int y);
  void copyTileData();
  void updateTileViewer();
  // Called from emulation thread with emu_mutex held

  // Called from main thread WITHOUT emu_mutex
};