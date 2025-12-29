#include "../include/all.hpp"
#include "raylib.h"
#include <cstdint>

void ERROR(std::string err) {
  fmt::println("FATAL ERROR: {} (you fucked up)", err);
  exit(-1);
}

void delay(int32_t ms) {
    WaitTime(ms/1000.0f);
}
