const std = @import("std");

pub fn build(b: *std.Build) void {
    // Standard target options allows the person running `zig build` to choose
    // what target to build for. Here we do not override the defaults, which
    // means any target is allowed, and the default is native.
    const target = b.standardTargetOptions(.{});

    // Standard optimization options allow the person running `zig build` to select
    // between Debug, ReleaseSafe, ReleaseFast, and ReleaseSmall.
    const optimize = b.standardOptimizeOption(.{});

    // Create the executable
    const exe = b.addExecutable(.{
        .name = "raylib_game",
        .target = target,
        .optimize = optimize,
    });

    // Add C source files
    exe.addCSourceFiles(.{
        .files = &[_][]const u8{
            "src/main.c",
        },
        .flags = &[_][]const u8{
            "-std=c99",
            "-Wall",
            "-Wextra",
        },
    });

    // Link raylib
    exe.linkSystemLibrary("raylib");
    exe.linkLibC();

    // Platform-specific linking
    switch (target.result.os.tag) {
        .windows => {
            exe.linkSystemLibrary("winmm");
            exe.linkSystemLibrary("gdi32");
            exe.linkSystemLibrary("opengl32");
        },
        .macos => {
            exe.linkFramework("OpenGL");
            exe.linkFramework("Cocoa");
            exe.linkFramework("IOKit");
            exe.linkFramework("CoreFoundation");
            exe.linkFramework("CoreVideo");
        },
        .linux => {
            exe.linkSystemLibrary("GL");
            exe.linkSystemLibrary("m");
            exe.linkSystemLibrary("pthread");
            exe.linkSystemLibrary("dl");
            exe.linkSystemLibrary("rt");
            exe.linkSystemLibrary("X11");
        },
        else => {},
    }

    // Install the executable
    b.installArtifact(exe);

    // Create a run step
    const run_cmd = b.addRunArtifact(exe);
    run_cmd.step.dependOn(b.getInstallStep());

    // Allow passing arguments to the application
    if (b.args) |args| {
        run_cmd.addArgs(args);
    }

    // Create a run step that can be invoked with `zig build run`
    const run_step = b.step("run", "Run the app");
    run_step.dependOn(&run_cmd.step);

    // Create unit tests (if you have any)
}
