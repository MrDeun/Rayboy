#include <stdint.h>
typedef struct z80 {
  struct _clock {
    int32_t m;
    int32_t t;
  };

  struct _register {
    uint8_t a, b, c, d, e, h, l, f;
    uint16_t pc, sp;
    int32_t m, t;
  };

} z80;