#include "../include/all.hpp"
#include "fmt/core.h"
#include "fmt/format.h"
#include "imgui.h"
#include <cassert>
#include <pthread.h>

#include "raylib-cpp.hpp"
#include "raylib.h"
#include "rlImGui.h"

static emu_context ctx;
emu_context *emu_get_context() { return &ctx; }
void delay(uint32_t ms) { WaitTime(ms / 1000.0f); }

void *cpu_run(void *p) {
  cpu_init();

  ctx.running = true;
  ctx.paused = false;
  ctx.ticks = 0;

  while (ctx.running) {
    if (ctx.paused) {
      delay(10);
      continue;
    }

    if (!cpu_step()) {
      fmt::println("CPU stopped...");
      return 0;
    }

    ctx.ticks++;
  }
}

void MainMenu() {
  if (ImGui::BeginMainMenuBar()) {
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
  pthread_t gb_thread = {0};
  SetTargetFPS(60);

  rlImGuiSetup(true);

  if (cart_load("01-special.gb")) {
    if (!pthread_create(&gb_thread, nullptr, cpu_run, nullptr)) {
      fmt::println("ERROR: Failed to run Gameboy Thread");
    }
  }

  while (!WindowShouldClose()) {
    BeginDrawing();
    rlImGuiBegin();
    ClearBackground(raylib::Color::White());

    DrawRectangle(0, 0, 160 * ctx.EmulatorScale, 140 * ctx.EmulatorScale,
                  raylib::Color::DarkBrown());

    MainMenu();

    rlImGuiEnd();
    EndDrawing();
  }

  rlImGuiShutdown();

  CloseWindow();
  return 0;
}
