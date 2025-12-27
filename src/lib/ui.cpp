#include "../include/all.hpp"
#include "Color.hpp"
#include "Rectangle.hpp"
#include "fmt/core.h"
#include "imgui.h"
#include "raylib.h"
#include "Image.hpp"
#include "rlImGui.h"
#include <array>
extern emu_context ctx;

static RenderTexture2D debugTexture;
int scale = 4;
static bool showDebugWindow = true;
const std::array<raylib::Color, 4> tile_colors{
    raylib::Color::White(), raylib::Color::LightGray(),
    raylib::Color::DarkGray(), raylib::Color::Black()};

void RayboyUI::Setup() {
  m_window.SetSize(1280, 720);
  m_window.SetTitle("Rayboy - The Gameboy Emulator");
  scale = EmulatorScale;
  SetConfigFlags(FLAG_MSAA_4X_HINT);
  int debugWidth = 16 * 8 * EmulatorScale;
  int debugHeight = 32 * 8 * EmulatorScale;

  debugTexture = LoadRenderTexture(debugWidth * debugWidth,
                                   debugHeight * (64 * EmulatorScale));

  rlImGuiSetup(true);
}

void delay(uint32_t ms) { WaitTime(ms / 1000.0f); }

Image RayboyUI::displayTile(uint16_t beginAddress, uint16_t tileNum, int scale) {
  // Create an 8x8 image for the tile
  Image img = GenImageColor(8 * scale, 8 * scale, BLACK);

  for (int tileY = 0; tileY < 16; tileY += 2) {
    uint8_t b1 = bus_read(beginAddress + (tileNum * 16) + tileY);
    uint8_t b2 = bus_read(beginAddress + (tileNum * 16) + tileY + 1);

    // fmt::println("ImageByte1 = {:b}, ImageByte2 = {:b}",b1,b2);

    for (int bit = 7; bit >= 0; bit--) {
      uint8_t high = !!(b2 & (1 << bit)) << 1;
      uint8_t low = !!(b1 & (1 << bit));
      uint8_t colorIndex = high | low;

      // Draw a scaled pixel
      for (int sy = 0; sy < scale; sy++) {
        for (int sx = 0; sx < scale; sx++) {
          int pixelX = ((7 - bit) * scale) + sx;
          int pixelY = (tileY / 2 * scale) + sy;
          ImageDrawPixel(&img, pixelX, pixelY, tile_colors[colorIndex]);
        }
      }
    }
  }

  return img;
}

bool RayboyUI::checkHold() {
  if (isHoldRequired) {
    return IsKeyPressed(KEY_ENTER);
  } else {
    return true;
  }
}

void RayboyUI::drawDebug() {
  BeginTextureMode(debugTexture);
  ClearBackground(BLACK);

  int xDraw = 0;
  int yDraw = 0;
  int tileNum = 0;
  uint16_t address = 0x8000;

  for (int y = 0; y < 24; y++) {
    for (int x = 0; x < 16; x++) {
      Image tileImg = displayTile(address, tileNum, EmulatorScale);
      Texture2D tileTex = LoadTextureFromImage(tileImg);

      DrawTexture(tileTex, xDraw, yDraw, WHITE);

      UnloadTexture(tileTex);
      UnloadImage(tileImg);

      xDraw += (8 * EmulatorScale);
      tileNum++;
    }
    yDraw += (8 * EmulatorScale);
    xDraw = 0;
  }

  EndTextureMode();
}
void drawTileDebug() {

  ImGui::SetNextWindowSize(ImVec2(400, 600), ImGuiCond_FirstUseEver);
  ImGui::SetNextWindowPos(ImVec2(1280 - 410, 10),
                          ImGuiCond_FirstUseEver);

  if (ImGui::Begin("Tile Debugger", &showDebugWindow)) {
    ImGui::Text("VRAM Tiles (0x8000-0x97FF)");
    ImGui::Text("384 tiles total");
    ImGui::Separator();

    // Display the debug texture
    rlImGuiImageRect(&debugTexture.texture, debugTexture.texture.width,
                     debugTexture.texture.height,
                     (Rectangle){0, 0, (float)debugTexture.texture.width,
                                 (float)-debugTexture.texture.height});

    // Optional: Add scale control
    ImGui::Separator();
    if (ImGui::SliderInt("Scale", &scale, 1, 4)) {
      // Recreate texture with new scale
      UnloadRenderTexture(debugTexture);
      debugTexture = LoadRenderTexture((16 * 8 * scale) + (16 * scale),
                                       (32 * 8 * scale) + (64 * scale));
    }
  }
  ImGui::End();
}
void RayboyUI::mainMenu() {
  if (ImGui::BeginMainMenuBar()) {
    // if (ImGui::BeginMenu("Rescale")) {
    //   if (ImGui::MenuItem("1x")) {
    //     ctx.EmulatorScale = 1;
    //   }
    //   if (ImGui::MenuItem("2x")) {
    //     ctx.EmulatorScale = 2;
    //   }
    //   if (ImGui::MenuItem("4x")) {
    //     ctx.EmulatorScale = 4;
    //   }
    //   if (ImGui::MenuItem("8x")) {
    //     ctx.EmulatorScale = 8;
    //   }
    //   ImGui::EndMenu();
    // }

    if (ImGui::BeginMenu("Emulation Status")) {
      auto hold_str =
          fmt::format("Hold To Run: {}", isHoldRequired ? "True" : "False");
      if (ImGui::MenuItem(hold_str.c_str())) {
        isHoldRequired = !isHoldRequired;
      }

      ImGui::EndMenu();
    }

    ImGui::EndMainMenuBar();
  }
}

void RayboyUI::Draw() {

  mainMenu();
  DrawRectangle(0, 0, 160 * EmulatorScale, 144 * EmulatorScale,
                raylib::Color::DarkBrown());
  drawDebug();
  drawTileDebug();
  if (m_emuCtx->running && checkHold()) {
    cpu_step();
  }
}
