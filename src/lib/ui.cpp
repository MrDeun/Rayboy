#include "../include/all.hpp"
#include "fmt/core.h"
#include "imgui.h"
#include "raylib-cpp.hpp"
#include "raylib.h"
#include "rlImGui.h"
#include <array>
#include <cstdint>

#include "imgui.h"
#include "raylib.h"
#include "rlImGui.h"

static const int scale = 4;

auto RayboyUI::setup() -> void {
  InitWindow(_ScreenWidth + (16 * 8 * scale) + 40, _ScreenHeight,
             "Rayboy - The Gameboy Emulator");
  SetTargetFPS(60);
  rlImGuiSetup(true);
  int dbgWidth = (16 * 8 * scale) + (16 * scale);
  int dbgHeight = (32 * 8 * scale) + (64 * scale);

  debugImage = GenImageColor(dbgWidth, dbgHeight, (Color){17, 17, 17, 255});
  debugTexture = LoadTextureFromImage(debugImage);
}

auto RayboyUI::shutdown() -> void {
  UnloadTexture(debugTexture);
  UnloadImage(debugImage);
  rlImGuiShutdown();
  CloseWindow();
}

auto RayboyUI::displayTile(uint16_t startLocation, uint16_t tileNum, int x,
                           int y) -> void {
  for (int tileY = 0; tileY < 16; tileY += 2) {
    uint8_t b1 = bus_read(startLocation + (tileNum * 16) + tileY);
    uint8_t b2 = bus_read(startLocation + (tileNum * 16) + tileY + 1);

    for (int bit = 7; bit >= 0; bit--) {
      uint8_t hi = !!(b1 & (1 << bit)) << 1;
      uint8_t lo = !!(b2 & (1 << bit));
      uint8_t color = hi | lo;

      int px = x + ((7 - bit) * scale);
      int py = y + ((tileY / 2) * scale);

      for (int sy = 0; sy < scale; sy++) {
        for (int sx = 0; sx < scale; sx++) {
          ImageDrawPixel(&debugImage, px + sx, py + sy, tileColors[color]);
        }
      }
    }
  }
}
auto RayboyUI::copyTileData() -> void {
  std::vector<uint8_t> temp_data;
  // Read all tile data at once
  for (uint16_t addr = 0x8000; addr < 0x9800; addr++) {
    temp_data.push_back(bus_read(addr));
  }

  // Quick swap with render data
  {
    std::lock_guard<std::mutex> lock(render_mutex);
    tile_data_copy = std::move(temp_data);
  }
}

auto RayboyUI::displayTileFromCopy(uint16_t tileNum, int x, int y) -> void {
  for (int tileY = 0; tileY < 16; tileY += 2) {
    uint16_t offset = tileNum * 16 + tileY;
    if (offset + 1 >= tile_data_copy.size())
      break;

    uint8_t b1 = tile_data_copy[offset];
    uint8_t b2 = tile_data_copy[offset + 1];

    for (int bit = 7; bit >= 0; bit--) {
      uint8_t hi = !!(b1 & (1 << bit)) << 1;
      uint8_t lo = !!(b2 & (1 << bit));
      uint8_t color = hi | lo;

      int px = x + ((7 - bit) * scale);
      int py = y + ((tileY / 2) * scale);

      for (int sy = 0; sy < scale; sy++) {
        for (int sx = 0; sx < scale; sx++) {
          ImageDrawPixel(&debugImage, px + sx, py + sy, tileColors[color]);
        }
      }
    }
  }
}

auto RayboyUI::updateTileViewer() -> void {
  ImageClearBackground(&debugImage, (Color){17, 17, 17, 255});

  int xDraw = 0;
  int yDraw = 0;
  int tileNum = 0;

  uint16_t addr = 0x8000;

  // 384 tiles (24 x 16)
  for (int y = 0; y < 24; y++) {
    for (int x = 0; x < 16; x++) {
      displayTile(addr, tileNum, xDraw + (x * scale),
                  yDraw + (y * scale));
      xDraw += (8 * scale);
      tileNum++;
    }

    yDraw += (8 * scale);
    xDraw = 0;
  }

  UpdateTexture(debugTexture, debugImage.data);
}
auto RayboyUI::draw() -> void {
  BeginDrawing();
  ClearBackground(BLACK);
  rlImGuiBegin();
  ImGui::Begin("Tile Viewer");
  ImGui::Image((ImTextureID)(uintptr_t)debugTexture.id,
               ImVec2((float)debugTexture.width, (float)debugTexture.height));
  ImGui::End();
  rlImGuiEnd();
  EndDrawing();
}