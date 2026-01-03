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
static bool showDebugWindow = true;
const std::array<raylib::Color, 4> tile_colors{
    raylib::Color::White(), raylib::Color::LightGray(),
    raylib::Color::DarkGray(), raylib::Color::Black()};

void RayboyUI::Setup() {
  m_window.SetSize(1280, 720);
  m_window.SetTitle("Rayboy - The Gameboy Emulator");
  SetConfigFlags(FLAG_MSAA_4X_HINT);
  int debugWidth = 16 * 8 * EmulatorScale;
  int debugHeight = 32 * 8 * EmulatorScale;

  debugTexture = LoadRenderTexture(debugWidth * debugWidth,
                                   debugHeight * (64 * EmulatorScale));

  rlImGuiSetup(true);
}

void delay(uint32_t ms) { WaitTime(ms / 1000.0f); }


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
  DrawRectangle(0, 0, 160*EmulatorScale, 144*EmulatorScale, DARKGRAY);
}
