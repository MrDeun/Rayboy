#include "../include/all.hpp"
#include "fmt/core.h"
#include <cstdint>
#include <cstdlib>

struct ram_context {
    uint8_t wram[0x2000];
    uint8_t hram[0x80];
};

static ram_context ctx;

uint8_t wram_read(uint16_t address) {
    auto err = fmt::format("ERROR: Invalid wram read address 0x{:X}",address);
    address -= 0xC000;
    if (address >= 0x2000) {
        fmt::println(err);
        exit(-1);
    }
    return ctx.wram[address];
}

void wram_write(uint16_t address, uint8_t value) {
    auto err = fmt::format("ERROR: Invalid wram write address 0x{:X}",address);
    address -= 0xC000;
    if(address >= 0x2000){
        fmt::println(err);
        exit(-1);
    }
    ctx.wram[address] = value;
}

uint8_t hram_read(uint16_t address) {
    auto err = fmt::format("ERROR: Invalid hram read address 0x{:X}",address);
    address -= 0xFF80;
    if (address >= 0x2000) {
        fmt::println(err);
        exit(-1);
    }
    return ctx.hram[address];
}

void hram_write(uint16_t address, uint8_t value) {
    auto err = fmt::format("ERROR: Invalid hram write address 0x{:X}",address);
    address -= 0xFF80;
    if(address >= 0x2000){

    }
    ctx.hram[address] = value;
}
