#include "../include/all.hpp"
#include "fmt/core.h"
#include "imgui.h"
#include <cassert>

#include "raylib-cpp.hpp"
#include "raylib.h"
#include "rlImGui.h"

static emu_context ctx = {0};
emu_context *emu_get_context() { return &ctx; }


int emu_run(int argc, char **argv) {
    RayboyUI _ui(emu_get_context());
  _ui.Setup();

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

    _ui.Draw();


    rlImGuiEnd();
    EndDrawing();
  }

  rlImGuiShutdown();

  CloseWindow();
  return 0;
}
