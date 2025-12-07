#include "../include/all.hpp"
#include "fmt/core.h"
#include "imgui.h"
#include <cassert>

#include "raylib-cpp.hpp"
#include "raylib.h"
#include "rlImGui.h"

static emu_context ctx;
emu_context *emu_get_context() { return &ctx; }
void delay(uint32_t ms);

bool checkHold(){
    if(ctx.isHoldRequired) {
        return raylib::Keyboard::IsKeyDown(KEY_ENTER);
    } else {
        return true;
    }
}

void MainMenu(raylib::Window &window) {
  if (ImGui::BeginMainMenuBar()) {
    if (ImGui::BeginMenu("Rescale")) {
      if (ImGui::MenuItem("1x")) {
        ctx.EmulatorScale = 1;
      }
      if (ImGui::MenuItem("2x")) {
        ctx.EmulatorScale = 2;
      }
      if (ImGui::MenuItem("4x")) {
        ctx.EmulatorScale = 4;
      }
      if (ImGui::MenuItem("8x")) {
        ctx.EmulatorScale = 8;
      }
      ImGui::EndMenu();
    }

    if (ImGui::BeginMenu("Emulation Status")) {
        auto hold_str = fmt::format("Hold To Run: {}", ctx.isHoldRequired ? "True" : "False");
        if (ImGui::MenuItem(hold_str.c_str())) {
            ctx.isHoldRequired = !ctx.isHoldRequired;
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
    DrawRectangle(0, 0, 160 * (ctx.EmulatorScale + 20), 140 * ctx.EmulatorScale,
                  raylib::Color::DarkBrown());

    if (ctx.running && checkHold()) {
      cpu_step();
    }

    rlImGuiEnd();
    EndDrawing();
  }

  rlImGuiShutdown();

  CloseWindow();
  return 0;
}
