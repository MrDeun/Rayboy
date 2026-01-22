#include "../include/all.hpp"
#include "raylib.h"
#include <chrono>
#include <thread>
#include <cstdint>

void ERROR(std::string err) {
  fmt::println("FATAL ERROR: {} (you fucked up)", err);
  exit(-1);
}

uint32_t get_ticks(){
    static auto start = std::chrono::steady_clock::now();
    auto now = std::chrono::steady_clock::now();
    auto ms = std::chrono::duration_cast<std::chrono::milliseconds>(now - start);
    return static_cast<uint32_t>(ms.count());
}

void delay(int32_t ms) {
    std::this_thread::sleep_for(std::chrono::milliseconds(ms));
}
