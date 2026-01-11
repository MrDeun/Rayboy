#pragma once

#include "../include/all.hpp"
#include <cstdint>
#include <iostream>
#include <format>

static char dbg_msg[1024] = {0};
static int msg_size = 0;
void dbg_update() {
// fmt::println("bus_read(0xFF02) = {:X}",bus_read(0xff02));
  if (bus_read(0xFF02) == 0x81) {
    int8_t c = bus_read(0xFF01);

    dbg_msg[msg_size++] = c;
    bus_write(0xff02, 0);
  }
}

void dbg_print() {

    if (dbg_msg[0]) {
      auto debug_msg =  fmt::format("DEBUG: {}\n", dbg_msg);
      std::cerr << debug_msg;
    }
}
