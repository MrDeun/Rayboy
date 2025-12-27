#include "../include/all.hpp"

static timer_context ctx = {0};

void timer_init() { ctx.div = 0xAC00; }
void timer_tick() {
  uint16_t prev_div = ctx.div;
  ctx.div++;
  bool timer_update = false;

  switch (ctx.tac & (0b11)) {
  case 0b00:
    timer_update = (prev_div & (1 << 9)) && (!(ctx.div & (1 << 9)));
    break;
  case 0b01:
    timer_update = (prev_div & (1 << 3)) && (!(ctx.div & (1 << 3)));
    break;
  case 0b10:
    timer_update = (prev_div & (1 << 5)) && (!(ctx.div & (1 << 5)));
    break;
  case 0b11:
    timer_update = (prev_div & (1 << 7)) && (!(ctx.div & (1 << 7)));
    break;
  }

  if(timer_update && ctx.tac & (1<<2)){
      ctx.tima++;
      if (ctx.tima==0xff) {
          ctx.tima = ctx.tma;
          cpu_request_interupts(IT_TIMER);
      }
  }
}

void timer_write(uint16_t address, uint8_t value){
    switch (address) {
        case 0xff04:
            ctx.div = 0;
            break;
        case 0xff05:
            ctx.tima = value;
            break;
        case 0xff06:
            ctx.tma = value;
            break;
        case 0xff07:
            ctx.tac = value;
            break;
        default:
            ERROR("Illegal address in timer_write()");
    }
}

uint8_t timer_read(uint16_t address){
    switch (address) {
        case 0xff04:
            return ctx.div >> 8;
        case 0xff05:
            return ctx.tima ;
        case 0xff06:
            return ctx.tma;
        case 0xff07:
            return ctx.tac;
        default:
            ERROR("Illegal address in timer_read()");
    }
}
