#include "../include/all.hpp"
#include "fmt/core.h"
#include "imgui.h"
#include <array>
#include <cassert>
#include <cstddef>
#include <cstdint>
#include <cstdlib>
#include <mutex>
#include <pthread.h>
#include <thread>
#include <unistd.h>

#include "raylib-cpp.hpp"
#include "raylib.h"
#include "rlImGui.h"

static emu_context ctx = {0};
emu_context *emu_get_context() { return &ctx; }
std::array<uint8_t, 0x30> nintendo_logo{};

int emu_run(int argc, char **argv) {
  if (argc < 2) {
    fmt::println("Missing ROM path\n USAGE: Rayboy <path-to-rom>");
    exit(EXIT_SUCCESS);
  }

  RayboyUI ui;
  ui.setup();

  std::mutex emu_mutex;
  EmulatorShared shared = {.mutex = &emu_mutex, .emu_ctx = &ctx};

  std::thread gb_thread;
  bool is_cart_loaded = cart_load(argv[1]);

  if (is_cart_loaded) {
    gb_thread = std::thread(cpu_run, &shared);
  }

  while (!ctx.die) {
    static int frame_count = 0;
    if (frame_count++ % 2 == 0) { // Update every 2 frames
      std::lock_guard<std::mutex> lock(*shared.mutex);
      ui.copyTileData();
    }

    // Update and draw WITHOUT the emulation lock
    ui.updateTileViewer();
    ui.draw();
  }

  // Clean shutdown
  if (gb_thread.joinable()) {
    {
      std::lock_guard<std::mutex> lock(*shared.mutex);
      ctx.running = false;
    }
    gb_thread.join();
  }

  ui.shutdown();
  return 0;
}
