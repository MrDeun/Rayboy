

#include <stdint.h>
#include "helpers.c"
typedef struct mmu {
  uint8_t memory[UINT16_MAX];
} mmu;

void wb(mmu *mem_unit, uint16_t address, uint8_t val) {
  mem_unit->memory[address] = val;
}

void ww(mmu *mem_unit, uint16_t address, uint16_t val) {
    U8x2 temp = {0};
    temp.u16 = val;
    mem_unit->memory[address] = temp.u8[0];
    mem_unit->memory[address+1] = temp.u8[1];
}

uint8_t rb(mmu *mem_unit, uint16_t address) {
    return mem_unit->memory[address];
}

uint16_t rw(mmu* mem_unit, uint16_t address){
    U8x2 temp = {0};
    temp.u8[0] = mem_unit->memory[address];
    temp.u8[1] = mem_unit->memory[address+1];
    return temp.u16;
}