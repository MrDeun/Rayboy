#include "../include/all.hpp"
#include "Color.hpp"
#include "Rectangle.hpp"
#include "Vector2.hpp"
#include "imgui.h"
#include "raylib.h"
#include "rlImGui.h"
#include <array>
extern emu_context ctx;

static RenderTexture2D debugTexture;

static std::array<raylib::Color, 4> tile_colors = {
    raylib::Color::White(), raylib::Color::LightGray(),
    raylib::Color::DarkGray(), raylib::Color::Black()};

void UiSetup() {
  SetConfigFlags(FLAG_MSAA_4X_HINT);
  ctx.window.SetSize(1280, 720);
  ctx.window.SetTitle("Rayboy - The Gameboy Emulator");

  int debugWidth = 16 * 8 * ctx.EmulatorScale;
  int debugHeight = 32 * 8 * ctx.EmulatorScale;

  debugTexture = LoadRenderTexture(debugWidth * debugWidth,
                                   debugHeight * (64 * ctx.EmulatorScale));

  rlImGuiSetup(true);
}

void delay(uint32_t ms) { WaitTime(ms / 1000.0f); }

raylib::Rectangle displayTile(uint16_t beginAddress, uint16_t tileNum, int x,
                              int y) {}

void drawDebug() {
  int xDraw = 0;
  int yDraw = 0;
  int tileNum = 0;

  raylib::Rectangle rect;
  rect.SetX(0);
  rect.SetY(0);
  rect.SetWidth(0);
  rect.SetHeight(0);
}

void MainMenu() {
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
          fmt::format("Hold To Run: {}", ctx.isHoldRequired ? "True" : "False");
      if (ImGui::MenuItem(hold_str.c_str())) {
        ctx.isHoldRequired = !ctx.isHoldRequired;
      }

      ImGui::EndMenu();
    }

    ImGui::EndMainMenuBar();
  }
}