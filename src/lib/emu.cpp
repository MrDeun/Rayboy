#include "../include/all.hpp"
#include "Vector2.hpp"
#include "Window.hpp"
#include "fmt/core.h"
#include "fmt/format.h"
#include "imgui.h"
#include <cassert>

#include "raylib-cpp.hpp"
#include "raylib.h"
#include "rlImGui.h"

static emu_context ctx;
emu_context *emu_get_context() { return &ctx; }
void delay(uint32_t ms);

void OpenGameboyROM() {
  ImGui::OpenPopup("EnterPath");
  if (ImGui::BeginPopup("EnterPath")) {
    static char path_buffer[0xFF];
    ImGui::InputText("Path", path_buffer, IM_ARRAYSIZE(path_buffer));
    if (ImGui::Button("Open")) {
      if (cart_load(path_buffer)) {
        ctx.running = true;
        cpu_init();
      }
      ImGui::CloseCurrentPopup();
    }
  }
  return;
}

void MainMenu(raylib::Window &window) {
  if (ImGui::BeginMainMenuBar()) {
    if (ImGui::BeginMenu("File")) {
      if (ImGui::MenuItem("Open", "Ctrl+O")) {
        // TODO: Loading a GameBoy cartridge
        // OpenGameboyROM();
        OpenGameboyROM();
      }

      if (ImGui::MenuItem("Check Tetris")) {
        if (cart_load("Tetris.gb")) {
          ctx.running = true;
          cpu_init();
        }
      }
      if (ImGui::MenuItem("Test Instruction Set")) {
        if (cart_load("cpu_instrs.gb")) {
          ctx.running = true;
          cpu_init();
        }
      }

      ImGui::EndMenu();
    }
    if (ImGui::BeginMenu("Rescale")) {
      if (ImGui::MenuItem("1x")) {
        ctx.EmulatorScale = 1;
      }
      if (ImGui::MenuItem("2x")) {
        ctx.EmulatorScale = 2;
      }
      if (ImGui::MenuItem("3x")) {
        ctx.EmulatorScale = 3;
      }
      if (ImGui::MenuItem("4x")) {
        ctx.EmulatorScale = 4;
      }
      ImGui::EndMenu();
    }

    ImGui::EndMainMenuBar();
    window.SetSize(160 * ctx.EmulatorScale, 144 * ctx.EmulatorScale);
  }
}

int emu_run(int argc, char **argv) {
  SetConfigFlags(FLAG_MSAA_4X_HINT);
  raylib::Window window(160, 144);
  window.SetTitle("Rayboy - The Gameboy Emulator");
  SetTargetFPS(60);

  rlImGuiSetup(true);

  if (argc > 1) {
    if (cart_load(argv[1])) {
      ctx.running = true;
      cpu_init();
    }
  }

  while (!WindowShouldClose()) {
    BeginDrawing();
    rlImGuiBegin();
    ClearBackground(raylib::Color::White());

    MainMenu(window);
    DrawRectangle(0, 0, 160 * ctx.EmulatorScale, 140 * ctx.EmulatorScale,
                  raylib::Color::DarkBrown());

    if (ctx.running && raylib::Keyboard::IsKeyDown(KEY_ENTER)) {
      cpu_step();
    }

    rlImGuiEnd();
    EndDrawing();
  }

  rlImGuiShutdown();

  CloseWindow();
  return 0;
}
