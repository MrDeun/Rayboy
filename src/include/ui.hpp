#pragma once
#include "../include/all.hpp"

#include "Rectangle.hpp"
#include "Window.hpp"
#include "common.hpp"
#include "emu.hpp"
#include <cstddef>
void delay(uint32_t ms);

struct RayboyUI {
private:
  emu_context *m_emuCtx = nullptr;
  void drawDebug();
  bool checkHold();
  void mainMenu();

public:
  raylib::Window m_window;
  static const int width = 1280;
  static const int height = 720;

  uint8_t EmulatorScale = 2;
  bool isHoldRequired = true;

  RayboyUI() = delete;
  RayboyUI(emu_context *ctx)
      : m_window(width, height, "Rayboy - Gameboy emulator built in Raylib") {
    if (!ctx) {
      ERROR("No emulation context provided in RayboyUI")
    }
    m_emuCtx = ctx;
  }

  void displayTile(uint16_t beginAddress, uint16_t tileNum, int x,
                                int y);
  void Draw();
  void Setup();
};
