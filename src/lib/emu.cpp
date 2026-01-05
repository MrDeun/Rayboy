#include "../include/all.hpp"
#include "fmt/core.h"
#include "imgui.h"
#include <cassert>
#include <cstddef>
#include <cstdint>
#include <array>
#include <cstdlib>
#include <pthread.h>

#include "raylib-cpp.hpp"
#include "raylib.h"
#include "rlImGui.h"

static emu_context ctx = {0};
emu_context *emu_get_context() { return &ctx; }
std::array<uint8_t,0x30> nintendo_logo{};

int emu_run(int argc, char **argv) {
    if (argc < 2) {
    fmt::println("Missing ROM path\n USAGE: Rayboy <path-to-rom>");
    exit(EXIT_SUCCESS);
  }
    InitWindow(1280, 720, "Rayboy - Gameboy Emulator written in raylib");
  SetTraceLogLevel(LOG_NONE);
  pthread_t gb_thread;
  fmt::println(argv[1]);
  bool is_cart_loaded = cart_load(argv[1]);
  rlImGuiSetup(true);
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

    DrawRectangle(0, 0, 160, 144, DARKBROWN);

    rlImGuiEnd();
    EndDrawing();
  }

  rlImGuiShutdown();

  CloseWindow();
  return 0;
}
