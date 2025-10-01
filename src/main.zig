//! By convention, main.zig is where your main function lives in the case that
//! you are building an executable. If you are making a library, the convention
//! is to delete this file and start with root.zig instead.

const rl = @import("raylib");
const rg = @import("raygui");
const std = @import("std");

/// This imports the separate module containing `root.zig`. Take a look in `build.zig` for details.
const lib = @import("Rayboy_lib");

var Emulator_ScreenScale: i32 = 1;

pub fn RescaleEmulatorScreen() void {
    Emulator_ScreenScale += 1;
    if (Emulator_ScreenScale > 4) {
        Emulator_ScreenScale = 1;
    }
    std.debug.print("Scale {}\n", .{Emulator_ScreenScale});
}

pub fn main() !void {
    const screenWidth = 800;
    const screenHeight = 450;

    rl.initWindow(screenWidth, screenHeight, "raylib-zig [core] example - basic window");
    defer rl.closeWindow(); // Close window and OpenGL context
    rl.setTargetFPS(60); // Set our game to run at 60 frames-per-second

    while (!rl.windowShouldClose()) { // Detect window close button or ESC key
        rl.beginDrawing();
        defer rl.endDrawing();

        rl.clearBackground(.white);
        const txt = "Window initrilized!";
        const text_size = rl.measureTextEx(try rl.getFontDefault(), txt, 20, 1);
        rl.drawText(txt, @intFromFloat(screenWidth / 2 - text_size.x / 2), @intFromFloat(screenHeight / 2), 20, .black);

        if (rg.button(.{ .x = 24, .y = 24, .width = 50, .height = 50 }, "Rescale")) {
            RescaleEmulatorScreen();
        }
    }
}
