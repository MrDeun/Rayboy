#include "../include/all.hpp"
#include "fmt/core.h"

static char dbg_msg[1024] = {0};
static int msg_size = 0;

void dbg_update(){
    if(bus_read(0xff02) == 0x81){
        char c = bus_read(0xff01);
        dbg_msg[msg_size++] = c;
        bus_write(0xff02, 0);
    }
}

void dbg_print(){
    if (dbg_msg[0]) {
        fmt::println("DEBUG: {}",dbg_msg);
    }
}