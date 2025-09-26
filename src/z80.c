#include <stdint.h>

const uint8_t FLAG_ZERO = 0x80;
const uint8_t FLAG_SUBSTRACT = 0x40;
const uint8_t FLAG_HALFCARRY = 0x20;
const uint8_t FLAG_CARRY = 0x10;

struct z80_clock {
  int32_t m;
  int32_t t;
};

struct z80_register {
  uint8_t a, b, c, d, e, f, h, l;
  uint16_t pc, sp;
  int32_t m, t;
};
typedef struct z80 {
  struct z80_clock _clock;
  struct z80_register _register;
} z80;