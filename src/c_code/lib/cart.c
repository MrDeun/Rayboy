#include "../include/cart.h"
#include <stdint.h>

typedef struct {
  char filename[1024];
  uint32_t rom_size;
  u8 *rom_data;
  rom_header *header;
} cart_context;

static cart_context ctx;
static const char *ROM_TYPES[UINT8_MAX+1] = {
    [0x00] = "ROM ONLY",
    [0x01] = "MBC1",
    [0x02] = "MBC1+RAM",
    [0x03] = "MBC1+RAM+BATTERY",
    [0x05] = "MBC2",
    [0x06] = "MBC2+BATTERY",
    [0x08] = "ROM+RAM 9",
    [0x09] = "ROM+RAM+BATTERY 9",
    [0x0B] = "MMM01",
    [0x0C] = "MMM01+RAM",
    [0x0D] = "MMM01+RAM+BATTERY",
    [0x0F] = "MBC3+TIMER+BATTERY",
    [0x10] = "MBC3+TIMER+RAM+BATTERY 10",
    [0x11] = "MBC3",
    [0x12] = "MBC3+RAM 10",
    [0x13] = "MBC3+RAM+BATTERY 10",
    [0x19] = "MBC5",
    [0x1A] = "MBC5+RAM",
    [0x1B] = "MBC5+RAM+BATTERY",
    [0x1C] = "MBC5+RUMBLE",
    [0x1D] = "MBC5+RUMBLE+RAM",
    [0x1E] = "MBC5+RUMBLE+RAM+BATTERY",
    [0x20] = "MBC6",
    [0x22] = "MBC7+SENSOR+RUMBLE+RAM+BATTERY",
    [0xFC] = "POCKET CAMERA",
    [0xFD] = "BANDAI TAMA5",
    [0xFE] = "HuC3",
    [0xFF] = "HuC1+RAM+BATTERY",
};

static const char* NEW_LICENSE_CODES[UINT8_MAX];
