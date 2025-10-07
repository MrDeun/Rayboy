#include "../include/bus.hpp"

#include <cstdint>
uint8_t bus_read(uint16_t address){
    if (address > 0x8000) {
        cart_read(address);
    }

    NO_IMPL
}
void bus_write(uint16_t address, uint8_t value){
    if (address > 0x8000) {
        cart_write(address,  value);
    }

    NO_IMPL
}