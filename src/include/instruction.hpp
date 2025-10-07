#pragma once

#include "common.hpp"

enum cond_type {};
enum in_type {};
enum addr_mode {};
enum reg_type{};

struct instruction {
    in_type type;
    addr_mode mode;
    reg_type reg1;
    reg_type reg2;
    cond_type cond;
    uint8_t param;
};