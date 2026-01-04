#pragma once
#include "../include/all.hpp"

#include "Rectangle.hpp"
#include "Window.hpp"
#include "common.hpp"
#include "emu.hpp"
#include "raylib.h"
#include <cstddef>
#include <cstdint>
#include <array>
void delay(uint32_t ms);

enum class GBColor : uint8_t {
  Black,
  LightGrey,
  DarkGrey,
  White,
};

struct RayboyUI {
private:
  emu_context *m_emuCtx = nullptr;
  // void drawDebug();
  // bool checkHold();
  void mainMenu();

public:
  raylib::Window m_window;
  static const int width = 1280;
  static const int height = 720;

  static const uint8_t EmulatorScale = 4;
  bool isHoldRequired = true;

  static std::array<GBColor, 768> decodeNintendoLogo(const uint8_t* logoData) {
    std::array<GBColor, 768> pixels;
    
    for (int i = 0; i < 48; i++) {
        uint8_t byte = logoData[i];
        
        // Each byte encodes 2 rows of 4 pixels each
        uint8_t highNibble = (byte >> 4) & 0x0F;
        uint8_t lowNibble = byte & 0x0F;
        
        // Decode each nibble (4 bits = 4 pixels, decompressed format)
        // Each bit represents one pixel (1 = black, 0 = white in logo)
        int baseIndex = i * 8;  // 8 pixels per byte
        
        // First row (high nibble)
        for (int bit = 0; bit < 4; bit++) {
            bool isSet = (highNibble >> (3 - bit)) & 1;
            pixels[baseIndex + bit] = isSet ? GBColor::Black : GBColor::White;
        }
        
        // Second row (low nibble)
        for (int bit = 0; bit < 4; bit++) {
            bool isSet = (lowNibble >> (3 - bit)) & 1;
            pixels[baseIndex + 4 + bit] = isSet ? GBColor::Black : GBColor::White;
        }
    }
    
    return pixels;
}

  RayboyUI() = delete;
  RayboyUI(emu_context *ctx)
      : m_window(width, height, "Rayboy - Gameboy emulator built in Raylib") {
    if (!ctx) {
      ERROR("No emulation context provided in RayboyUI");
    }
    m_emuCtx = ctx;
  }
  // Image displayTile(uint16_t beginAddress, uint16_t tileNum, int scale);
  
  void Draw(const std::array<uint8_t,48>& nintendo_bytes);
  void Setup();
};
