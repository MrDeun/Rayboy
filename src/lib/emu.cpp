#include "../include/all.hpp"
#include "fmt/core.h"
#include "imgui.h"
#include <cassert>
#include <cstddef>
#include <cstdint>
#include <array>
#include <pthread.h>

#include "raylib-cpp.hpp"
#include "raylib.h"
#include "rlImGui.h"

static emu_context ctx = {0};
emu_context *emu_get_context() { return &ctx; }
std::array<uint8_t,0x30> nintendo_logo{};

int emu_run(int argc, char **argv) {
  SetTraceLogLevel(LOG_NONE);
  RayboyUI _ui(emu_get_context());
  _ui.Setup();
  pthread_t gb_thread;
  bool is_cart_loaded = cart_load(argv[1]);


    if (is_cart_loaded) {
      for (size_t i=0;i<nintendo_logo.size();i++) {
        nintendo_logo[i] = getROMHeader()->nintendo_logo[i];
      }
      pthread_create(&gb_thread, NULL,
        cpu_run, NULL);
    }

  while (!WindowShouldClose()) {
    BeginDrawing();
    rlImGuiBegin();
    ClearBackground(raylib::Color::White());

    _ui.Draw(nintendo_logo);


    rlImGuiEnd();
    EndDrawing();
  }

  rlImGuiShutdown();

  CloseWindow();
  return 0;
}
