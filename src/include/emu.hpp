#pragma once

#include <mutex>
#include <atomic>
#include <vector>
struct EmulatorShared {
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
        
        std::lock_guard<std::mutex> lock(tile_swap_mutex);
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
};

int emu_run(int argc,char** argv);
