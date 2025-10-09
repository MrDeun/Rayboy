#pragma once

#include "common.hpp"

enum cond_type {};
enum in_type {
    IN_NOP,
    IN_LD,
    IN_RLCA,
    IN_INC,
    IN_DEC,
    IN_ADD,
    IN_RLA,
    IN_JR,
    IN_RRA,

};
enum addr_mode {};
enum reg_type{
    RT_NONE,
    RT_A,
    RT_F,
    RT_B,
    RT_C,
    RT_D,
    RT_E,
    RT_H,
    RT_L,
    RT_SP,
    RT_PC,
    RT_AF,
    RT_BC,
    RT_DE,
    RT_HL,
};

struct instruction {
    in_type type;
    addr_mode mode;
    reg_type reg1;
    reg_type reg2;
    cond_type cond;
    uint8_t param;
};
