#include <cstdint>
#include <fmt/core.h>
#include <imgui.h>
#include <raylib-cpp.hpp>
#include <raylib.h>
#include <rlImGui.h>

#include "include/cart.hpp"

uint8_t Emulator_Scale = 1;

void MainMenu(void) {
  if (ImGui::BeginMainMenuBar()) {
    if (ImGui::BeginMenu("File")) {
      if (ImGui::MenuItem("Open", "Ctrl+O")) {
        // TODO: Loading a GameBoy cartridge
        fmt::println("TEMP_DEBUG: Open File");
      }

      if (ImGui::MenuItem("Check Zelda")) {
        cart_load("Zelda.gb");
      }
      if (ImGui::MenuItem("Check Tetris")) {
        cart_load("Tetris.gb");
      }
      if (ImGui::MenuItem("Check Mario")) {
        cart_load("Mario.gb");
      }
      if (ImGui::MenuItem("Check Pokemon")) {
        cart_load("Pokemon.gb");
      }

      ImGui::EndMenu();
    }
    if (ImGui::BeginMenu("Rescale")) {
      if (ImGui::MenuItem("1x")) {
        Emulator_Scale = 1;
      }
      if (ImGui::MenuItem("2x")) {
        Emulator_Scale = 2;
      }
      if (ImGui::MenuItem("3x")) {
        Emulator_Scale = 3;
      }
      if (ImGui::MenuItem("4x")) {
        Emulator_Scale = 4;
      }
      ImGui::EndMenu();
    }

    ImGui::EndMainMenuBar();
  }
}

int main() {

  SetConfigFlags(FLAG_MSAA_4X_HINT);
  raylib::Window window(1280, 720);
  SetTargetFPS(60);

  rlImGuiSetup(true);

  while (WindowShouldClose() == false) {

    BeginDrawing();
    rlImGuiBegin();
    ClearBackground(raylib::Color::White());

    DrawRectangle(0, 0, 160 * Emulator_Scale, 140 * Emulator_Scale,
                  raylib::Color::DarkBrown());

    MainMenu();

    rlImGuiEnd();
    EndDrawing();
  }

  rlImGuiShutdown();

  CloseWindow();
  return 0;
}