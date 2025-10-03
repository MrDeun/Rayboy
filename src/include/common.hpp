
#include <stdint.h>
#include <stdbool.h>
typedef union {
  uint16_t u16;
  uint8_t u8[2];
} U8x2;

typedef uint8_t u8;
typedef uint16_t u16;
typedef uint32_t u32;


#define BIT_GET(a, n) ((a & (1 << n)) ? 1 : 0)
#define BIT_SET(a, n, on) (on ? a |= (1 << n) : a &= ~(1 << n))
#define BETWEEN(a, b, c) ((a >= b) && (a <= c))

void delay(uint32_t ms);