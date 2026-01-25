#pragma once


#include "common.hpp"
#include "bus.hpp"

#include <mutex>
#include <thread>
#include <atomic>
#include <vector>

struct EmulatorShared {

    uint32_t frames[2][XRES*YRES];

    std::atomic<int> write_index{0};
    std::atomic<int> read_index{1};
    std::atomic<bool> frame_ready{false};



  // ========== Thread Control ==========
    std::atomic<bool> running{false};
    std::atomic<bool> paused{false};
    std::atomic<bool> should_exit{false};
    
    // ========== Tile Data Transfer ==========
    std::vector<uint8_t> tile_cpu_buffer;
    std::vector<uint8_t> tile_transfer_buffer;
    std::vector<uint8_t> tile_ui_buffer;
    std::mutex tile_swap_mutex;
    std::atomic<bool> tile_needs_swap{false};
    
    // ========== VRAM Access Coordination ==========
    // Use atomic flag to coordinate between DMA and tile copying
    // This is lightweight since we only copy tiles once per second
    std::atomic<bool> copying_tiles{false};
    
    // ========== Statistics ==========
    std::atomic<uint64_t> frame_count{0};
    std::atomic<uint32_t> fps{0};
    
    EmulatorShared() {
        tile_cpu_buffer.resize(0x1800, 0);
        tile_transfer_buffer.resize(0x1800, 0);
        tile_ui_buffer.resize(0x1800, 0);
    }
    
    void signalTileDataReady() {
        tile_needs_swap.store(true, std::memory_order_release);
    }
    
    bool swapTileBuffers() {
        if (!tile_needs_swap.load(std::memory_order_acquire)) {
            return false;
        }
        
        std::unique_lock<std::mutex> lock(tile_swap_mutex, std::try_to_lock);
        if (!lock.owns_lock()) {
            return false;
        }
        
        tile_ui_buffer.swap(tile_transfer_buffer);
        tile_transfer_buffer.swap(tile_cpu_buffer);
        tile_needs_swap.store(false, std::memory_order_release);
        return true;
    }
    
    const std::vector<uint8_t>& getTileBuffer() const {
        return tile_ui_buffer;
    }
    
    std::vector<uint8_t>& getCpuTileBuffer() {
        return tile_cpu_buffer;
    }
    
    // Copy VRAM tile data safely - waits for any active DMA to complete
    void copyVramTiles(std::vector<uint8_t>& dest) {
        // Signal that we're copying tiles
        copying_tiles.store(true, std::memory_order_release);
        
        // Give DMA a chance to see the flag and pause if needed
        // In practice, DMA is very fast (160 bytes) so this is rarely needed
        std::this_thread::yield();
        
        // Copy all tile data from VRAM (0x8000-0x9800)
        for (uint16_t addr = 0x8000; addr < 0x9800; addr++) {
            dest[addr - 0x8000] = bus_read(addr);
        }
        
        // Done copying
        copying_tiles.store(false, std::memory_order_release);
    }
    
    // Check if tile copying is in progress (called from DMA)
    bool isCopyingTiles() const {
        return copying_tiles.load(std::memory_order_acquire);
    }
};

EmulatorShared* get_shared_emulator_state();

int emu_run(int argc,char** argv);
