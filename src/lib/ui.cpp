#include "../include/all.hpp"
#include "fmt/core.h"
#include "imgui.h"
#include "raylib.h"
#include "raylib-cpp.hpp"
#include "rlImGui.h"
#include <array>
#include <cstdint>
void delay(uint32_t ms){
    WaitTime((double)ms/1000.0);
}
