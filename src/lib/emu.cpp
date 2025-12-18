#include "../include/all.hpp"
#include "fmt/core.h"
#include "imgui.h"
#include <cassert>
#include <cstddef>
#include <pthread.h>

#include "raylib-cpp.hpp"
#include "raylib.h"
#include "rlImGui.h"

static emu_context ctx = {0};
emu_context *emu_get_context() { return &ctx; }


int emu_run(int argc, char **argv) {
    RayboyUI _ui(emu_get_context());
  _ui.Setup();
  pthread_t gb_thread;

  if (argc > 1) {
    if (cart_load(argv[1])) {
      pthread_create(&gb_thread, NULL, 
        cpu_run, NULL);
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
