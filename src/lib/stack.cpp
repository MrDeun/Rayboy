#include "../include/bus.hpp"
#include "../include/cpu.hpp"
#include <cstdint>
void stack_push(uint8_t data) {
  cpu_get_regs()->SP--;
  bus_write(cpu_get_regs()->SP, data);
}
void stack_push16(uint16_t data) {
  stack_push((data >> 8) & 0xFF);
  stack_push(data & 0xFF);
}

uint8_t stack_pop() {
    return bus_read(cpu_get_regs()->SP++);
}
uint16_t stack_pop16() {
    auto low = stack_pop();
    uint8_t high = stack_pop();

    return (high<<8) | low;
}
