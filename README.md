# Rayboy — Game Boy Emulator

A cycle-accurate Nintendo Game Boy emulator written from scratch in **C++17**, built with **raylib** for rendering. Developed as a deep dive into emulation, computer architecture, and real-time systems programming.

## Features

### Emulation

| Component | Description |
|-----------|-------------|
| **SM83 CPU** | Full instruction set (500+ opcodes including CB prefix), fetch-decode-execute pipeline with per-cycle timing |
| **PPU** | Cycle-accurate pixel-processing with FIFO-based rendering; background, window, and sprite layers (8×8 & 8×16) |
| **Memory Bus** | Unified address space bridging CPU, PPU, cartridge, RAM, I/O, DMA, and HRAM |
| **Cartridge** | ROM loading with Memory Bank Controller (MBC) support |
| **LCD** | Scroll, window positioning, monochrome palettes, STAT interrupt generation |
| **DMA** | OAM DMA transfer with VRAM conflict coordination |
| **Timer** | Game Boy divider and timer/counter with configurable clock rates |
| **Interrupts** | VBlank, LCD STAT, Timer, Serial, and Joypad interrupt handling |
| **Gamepad** | Keyboard-mapped input with directional and action buttons |

### Debug UI (raylib)

- **Live screen** — 160×144 game display rendered at 3× scale
- **Tile viewer** — real-time VRAM tile data visualization (384 tiles, 4-shade palette)
- **Stats panel** — FPS, cycle count, instruction count, CPU register state
- **Input viewer** — visual button state feedback

### Performance

- Multi-threaded architecture: CPU emulation on a dedicated thread, UI on the main thread
- Lock-free double-buffered frame buffer for zero-copy pixel transfer
- Triple-buffered tile data for non-blocking VRAM introspection
- DMA-aware tile copying that yields on conflict

## Build

### Prerequisites

- **CMake** ≥ 3.17
- **Clang** (Linux/macOS) or **LLVM Clang + Ninja** (Windows)
- Dependencies (raylib 5.0, fmt 10.2.1) are fetched automatically via `FetchContent`

### Presets

```bash
# Configure
cmake --preset clang-debug

# Build
cmake --build --preset clang-debug
```

Available presets: `clang-debug`, `clang-release` for Linux / Windows / macOS.

### Manual Build

```bash
cmake -B build -DCMAKE_BUILD_TYPE=Debug
cmake --build build
```

## Usage

```bash
./build/clang-debug/Rayboy path/to/rom.gb
```

### Controls

| Game Boy | Keyboard |
|----------|----------|
| D-Pad    | Arrow keys |
| A        | Z |
| B        | X |
| Start    | Enter |
| Select   | Backspace |

## Project Structure

```
src/
├── main.cpp             # Entry point
├── include/             # Headers
│   ├── cpu.hpp          # SM83 CPU
│   ├── ppu.hpp          # Picture Processing Unit
│   ├── bus.hpp          # Memory bus
│   ├── cart.hpp         # Cartridge / MBC
│   ├── lcd.hpp          # LCD control
│   ├── dma.hpp          # OAM DMA
│   ├── timer.hpp        # Timer/counter
│   ├── gamepad.hpp      # Input handling
│   ├── interupt.hpp     # Interrupt system
│   ├── instruction.hpp  # Opcode definitions
│   ├── emu.hpp          # Emulator shared state
│   ├── ui.hpp           # raylib debug UI
│   ├── dbg.hpp          # Debug utilities
│   ├── ram.hpp          # RAM
│   ├── stack.hpp        # Stack operations
│   └── io.hpp           # I/O registers
└── lib/                 # Implementations
    ├── cpu.cpp, cpu_fetch.cpp, cpu_proc.cpp, cpu_regs.cpp
    ├── ppu.cpp, ppu_pipeline.cpp, ppu_state_machine.cpp
    ├── bus.cpp, cart.cpp, emu.cpp
    ├── lcd.cpp, dma.cpp, timer.cpp, gamepad.cpp
    ├── interupt.cpp, instruction.cpp
    ├── ram.cpp, stack.cpp, io.cpp
    ├── ui.cpp, dbg.cpp, common.cpp
```
