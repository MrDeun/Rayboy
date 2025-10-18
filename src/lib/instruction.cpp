#include "../include/instruction.hpp"
#include <cstdint>

instruction instructions[0x100] = {
    [0x00] = {.type = IN_NOP, .mode = AM_IMP},
    [0x05] = {.type = IN_DEC, .mode = AM_R, .reg1 = RT_B},
    [0x0E] = {.type = IN_LD, .mode = AM_R_D8, .reg1 = RT_C},
    [0xAF] = {.type = IN_XOR, .mode = AM_R, .reg1 = RT_A},
    [0xC3] = {.type = IN_JP, .mode = AM_D16},
    [0xF3] = {.type = IN_DI}
};

instruction *get_instruction_by_opcode(uint8_t opcode) {
  if (instructions[opcode].type == IN_NONE) {
    return nullptr;
  }
  return &instructions[opcode];
}
