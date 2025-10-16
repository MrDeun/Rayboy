#include "../include/bus.hpp"
#include "fmt/core.h"

#include <cstdint>
uint8_t bus_read(uint16_t address){
    auto x = 0x8000;
    fmt::println("{}",address);
    if (address > 0x8000) {
        cart_read(address);
    }
    return 0;
    NO_IMPL("bus_read()")
}
void bus_write(uint16_t address, uint8_t value){
    if (address > 0x8000) {
        cart_write(address,  value);
    }
    return;
    NO_IMPL("bus_write()")
}