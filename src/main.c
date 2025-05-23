#include "raylib.h"

#define RAYGUI_IMPLEMENTATION

#include "raygui.h"
#include <stdint.h>

uint8_t Emulator_ScreenScale = 1;

#define BASE_SCREEN_HEIGHT 144 * Emulator_ScreenScale
#define BASE_SCREEN_WIDTH 160 * Emulator_ScreenScale

void RescaleEmulatorScreen(void) {
    // We want to rotate between scales 1 - 2
  Emulator_ScreenScale = (Emulator_ScreenScale + 1) % 2 + 1;
}

int main(void) {
  InitWindow(800, 600, "Hello world!");
  SetTargetFPS(60);
  while (!WindowShouldClose()) {
    BeginDrawing();
    ClearBackground(RAYWHITE);

    const char *text = "Window initialized!";
    const Vector2 text_size = MeasureTextEx(GetFontDefault(), text, 20, 1);
    DrawText(text, 400 - text_size.x / 2, 100 - text_size.y / 2, 20, BLACK);
    if(GuiButton((Rectangle){24,24,50,50}, "Rescale")){
        RescaleEmulatorScreen();
    };
    DrawRectangle(800 / 2 - BASE_SCREEN_WIDTH / 2,
                  600 / 2 - BASE_SCREEN_HEIGHT / 2, BASE_SCREEN_WIDTH,
                  BASE_SCREEN_HEIGHT, GRAY);
    EndDrawing();
  }

  CloseWindow();

  return 0;
}