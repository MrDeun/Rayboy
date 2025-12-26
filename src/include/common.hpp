#pragma once

#include <cstdint>
#include <fmt/core.h>

typedef union {
  uint16_t u16;
  uint8_t u8[2];
} U8x2;

#define BIT_GET(a, n) ((a & (1 << n)) ? 1 : 0)
#define BIT_SET(a, n, on) (on ? a |= (1 << n) : a &= ~(1 << n))
#define BETWEEN(a, b, c) ((a >= b) && (a <= c))

#define NO_IMPL(err)                                                           \
  {                                                                            \
    printf("NO IMPLEMENTION - REVIEW CODE\nReason: %s\n", err);                \
  }
#define ERROR(err)                                                             \
  {                                                                            \
    printf("FATAL ERROR: %s (you fucked up)\n");                               \
    exit(-1);                                                                  \
  }
void delay(uint32_t ms);