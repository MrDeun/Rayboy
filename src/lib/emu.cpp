#include "../include/all.hpp"
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

void testInstruction() {
  auto instruct1 = get_instruction_by_opcode(0x00);
  assert(instruct1 != nullptr);
  fmt::println("Instruction 1 = {:p}", fmt::ptr(instruct1));

  auto instruct2 = get_instruction_by_opcode(0x0E);
  assert(instruct2 != nullptr);
  fmt::println("Instruction 2 = {:p}", fmt::ptr(instruct2));

  assert(instruct1 != instruct2);
}

void MainMenu() {
  if (ImGui::BeginMainMenuBar()) {
    if (ImGui::BeginMenu("File")) {
      if (ImGui::MenuItem("Open", "Ctrl+O")) {
        // TODO: Loading a GameBoy cartridge
        fmt::println("TEMP_DEBUG: Open File");
      }

      if (ImGui::MenuItem("Check Zelda")) {
        if (cart_load("Zelda.gb")) {
          ctx.running = true;
          cpu_init();
        }
      }
      if (ImGui::MenuItem("Check Tetris")) {
        if (cart_load("Tetris.gb")) {
          ctx.running = true;
          cpu_init();
        }
      }
      if (ImGui::MenuItem("Check Mario")) {
        if (cart_load("Mario.gb")) {
          ctx.running = true;
          cpu_init();
        }
      }
      if (ImGui::MenuItem("Check Pokemon")) {
        if (cart_load("Pokemon.gb")) {
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
  }
}

int emu_run(int argc, char **argv) {
  SetConfigFlags(FLAG_MSAA_4X_HINT);
  raylib::Window window(1280, 720);
  SetTargetFPS(60);

  testInstruction();
  rlImGuiSetup(true);

  if (argc > 1) {
    if(cart_load(argv[1])){
        ctx.running = true;
        cpu_init();
    }
  }

  while (!WindowShouldClose()) {
    BeginDrawing();
    rlImGuiBegin();
    ClearBackground(raylib::Color::White());

    DrawRectangle(0, 0, 160 * ctx.EmulatorScale, 140 * ctx.EmulatorScale,
                  raylib::Color::DarkBrown());

    MainMenu();

    if (ctx.running && IsKeyPressed(KEY_ENTER)) {
      cpu_step();
    }

    rlImGuiEnd();
    EndDrawing();
  }

  rlImGuiShutdown();

  CloseWindow();
  return 0;
}
