#include "../include/all.hpp"
#include "Color.hpp"
#include "Rectangle.hpp"
#include "fmt/core.h"
#include "imgui.h"
#include "raylib.h"
#include "Image.hpp"
#include "rlImGui.h"
#include <array>
#include <cstdint>
extern emu_context ctx;

void RayboyUI::Setup() {
  m_window.SetSize(1280, 720);
  m_window.SetTitle("Rayboy - The Gameboy Emulator");
  SetConfigFlags(FLAG_MSAA_4X_HINT);
  int debugWidth = 16 * 8 * EmulatorScale;
  int debugHeight = 32 * 8 * EmulatorScale;
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
auto constexpr x = 0;
auto constexpr y = 0;
auto constexpr scale = 8;


void RayboyUI::Draw(const std::array<uint8_t,48>& nintendo_bytes) {
  mainMenu();
  DrawRectangle(0, 0, 160*EmulatorScale, 144*EmulatorScale, DARKGRAY);
  //Draw Nintendo Logo
  auto pixels = RayboyUI::decodeNintendoLogo(nintendo_bytes.data());
  for (int py = 0; py < 16; py++) {  // 16 rows total (2 rows of tiles)
        for (int px = 0; px < 48; px++) {
            int pixelIndex = py * 48 + px;
            GBColor color = pixels[pixelIndex];
            
            // Convert enum to grayscale value
            uint8_t gray = static_cast<int>(color);
            Color rayColor = { gray, gray, gray, 255 };
            
            // Draw scaled pixel
            DrawRectangle(
                x + px * scale,
                y + py * scale,
                scale,
                scale,
                rayColor
            );
        }
    }
}
