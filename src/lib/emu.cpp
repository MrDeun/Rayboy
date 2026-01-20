#include "../include/all.hpp"

#include <thread>
#include <print>

EmulatorShared* g_shared;

int emu_run(int argc, char **argv) {
    if (argc < 2) {
        fmt::println("Missing ROM path\nUSAGE: Rayboy <path-to-rom>");
        return 1;
    }
    
    if (!cart_load(argv[1])) {
        fmt::println("Failed to load cartridge: {}", argv[1]);
        return 1;
    }
    
    EmulatorShared shared;
    RayboyUI ui;
    ui.setup(&shared);
    g_shared = &shared;
    std::thread cpu_thread(cpu_run_threaded, &shared);
    
    while (!ui.shouldExit()) {
        ui.update();
        ui.draw();
    }
    
    shared.should_exit.store(true, std::memory_order_release);
    
    if (cpu_thread.joinable()) {
        cpu_thread.join();
    }
    g_shared = nullptr;
    ui.shutdown();
    
    return 0;
}