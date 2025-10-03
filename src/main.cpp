#include <imgui.h>
#include <raylib-cpp.hpp>
#include <raylib.h>
#include <rlImGui.h>

#include <cstdint>

uint8_t Emulator_Scale = 1;
void RotateEmulatorScale() {
  Emulator_Scale += 1;
  if (Emulator_Scale > 3)
    Emulator_Scale = 1;
}

int main() {

  SetConfigFlags(FLAG_MSAA_4X_HINT);
  raylib::Window window(1600, 1200);
  SetTargetFPS(60);

  rlImGuiSetup(true);

  while (WindowShouldClose() == false) {

    BeginDrawing();
    rlImGuiBegin();
    ClearBackground(raylib::Color::White());
    ImGui::ShowDemoWindow();    

    DrawRectangle(0, 0, 160 * Emulator_Scale, 140 * Emulator_Scale, DARKBROWN);

    ImGui::Begin("Rayboy");
    if (ImGui::Button("Rescale")) {
      RotateEmulatorScale();
    }
    ImGui::End();


    rlImGuiEnd();
    EndDrawing();
  }

  rlImGuiShutdown();

  CloseWindow();
  return 0;
}