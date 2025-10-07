#include "../include/cart.hpp"
#include "fmt/core.h"
#include <cstdint>
#include <fstream>
#include <map>

struct cart_context {
  char filename[1024];
  uint32_t rom_size;
  uint8_t *rom_data;
  rom_header *header;
};

static cart_context ctx;
const std::map<uint8_t, const std::string> ROM_TYPES = {
    {0x00, "ROM ONLY"},
    {0x01, "MBC1"},
    {0x02, "MBC1+RAM"},
    {0x03, "MBC1+RAM+BATTERY"},
    {0x05, "MBC2"},
    {0x06, "MBC2+BATTERY"},
    {0x08, "ROM+RAM 9"},
    {0x09, "ROM+RAM+BATTERY 9"},
    {0x0B, "MMM01"},
    {0x0C, "MMM01+RAM"},
    {0x0D, "MMM01+RAM+BATTERY"},
    {0x0F, "MBC3+TIMER+BATTERY"},
    {0x10, "MBC3+TIMER+RAM+BATTERY 10"},
    {0x11, "MBC3"},
    {0x12, "MBC3+RAM 10"},
    {0x13, "MBC3+RAM+BATTERY 10"},
    {0x19, "MBC5"},
    {0x1A, "MBC5+RAM"},
    {0x1B, "MBC5+RAM+BATTERY"},
    {0x1C, "MBC5+RUMBLE"},
    {0x1D, "MBC5+RUMBLE+RAM"},
    {0x1E, "MBC5+RUMBLE+RAM+BATTERY"},
    {0x20, "MBC6"},
    {0x22, "MBC7+SENSOR+RUMBLE+RAM+BATTERY"},
    {0xFC, "POCKET CAMERA"},
    {0xFD, "BANDAI TAMA5"},
    {0xFE, "HuC3"},
    {0xFF, "HuC1+RAM+BATTERY"},
};

const std::map<const uint8_t, const std::string> LICENSE_CODES = {
    {0x00, "None"},
    {0x01, "Nintendo"},
    {0x08, "Capcom"},
    {0x09, "HOT-B"},
    {0x0A, "Jaleco"},
    {0x0B, "Coconuts Japan"},
    {0x0C, "Elite Systems"},
    {0x13, "EA (Electronic Arts)"},
    {0x18, "Hudson Soft"},
    {0x19, "ITC Entertainment"},
    {0x1A, "Yanoman"},
    {0x1D, "Japan Clary"},
    {0x1F, "Virgin Games Ltd.3"},
    {0x24, "PCM Complete"},
    {0x25, "San-X"},
    {0x28, "Kemco"},
    {0x29, "SETA Corporation"},
    {0x30, "Infogrames5"},
    {0x31, "Nintendo"},
    {0x32, "Bandai"},
    {0x33, "Indicates that the New licensee code should be used instead."},
    {0x34, "Konami"},
    {0x35, "HectorSoft"},
    {0x38, "Capcom"},
    {0x39, "Banpresto"},
    {0x3C, "Entertainment Interactive (stub)"},
    {0x3E, "Gremlin"},
    {0x41, "Ubi Soft1"},
    {0x42, "Atlus"},
    {0x44, "Malibu Interactive"},
    {0x46, "Angel"},
    {0x47, "Spectrum HoloByte"},
    {0x49, "Irem"},
    {0x4A, "Virgin Games Ltd.3"},
    {0x4D, "Malibu Interactive"},
    {0x4F, "U.S. Gold"},
    {0x50, "Absolute"},
    {0x51, "Acclaim Entertainment"},
    {0x52, "Activision"},
    {0x53, "Sammy USA Corporation"},
    {0x54, "GameTek"},
    {0x55, "Park Place15"},
    {0x56, "LJN"},
    {0x57, "Matchbox"},
    {0x59, "Milton Bradley Company"},
    {0x5A, "Mindscape"},
    {0x5B, "Romstar"},
    {0x5C, "Naxat Soft16"},
    {0x5D, "Tradewest"},
    {0x60, "Titus Interactive"},
    {0x61, "Virgin Games Ltd.3"},
    {0x67, "Ocean Software"},
    {0x69, "EA (Electronic Arts)"},
    {0x6E, "Elite Systems"},
    {0x6F, "Electro Brain"},
    {0x70, "Infogrames5"},
    {0x71, "Interplay Entertainment"},
    {0x72, "Broderbund"},
    {0x73, "Sculptured Software6"},
    {0x75, "The Sales Curve Limited7"},
    {0x78, "THQ"},
    {0x79, "Accolade8"},
    {0x7A, "Triffix Entertainment"},
    {0x7C, "MicroProse"},
    {0x7F, "Kemco"},
    {0x80, "Misawa Entertainment"},
    {0x83, "LOZC G."},
    {0x86, "Tokuma Shoten"},
    {0x8B, "Bullet-Proof Software2"},
    {0x8C, "Vic Tokai Corp.17"},
    {0x8E, "Ape Inc.18"},
    {0x8F, "I’Max19"},
    {0x91, "Chunsoft Co.9"},
    {0x92, "Video System"},
    {0x93, "Tsubaraya Productions"},
    {0x95, "Varie"},
    {0x96, "Yonezawa10/S’Pal"},
    {0x97, "Kemco"},
    {0x99, "Arc"},
    {0x9A, "Nihon Bussan"},
    {0x9B, "Tecmo"},
    {0x9C, "Imagineer"},
    {0x9D, "Banpresto"},
    {0x9F, "Nova"},
    {0xA1, "Hori Electric"},
    {0xA2, "Bandai"},
    {0xA4, "Konami"},
    {0xA6, "Kawada"},
    {0xA7, "Takara"},
    {0xA9, "Technos Japan"},
    {0xAA, "Broderbund"},
    {0xAC, "Toei Animation"},
    {0xAD, "Toho"},
    {0xAF, "Namco"},
    {0xB0, "Acclaim Entertainment"},
    {0xB1, "ASCII Corporation or Nexsoft"},
    {0xB2, "Bandai"},
    {0xB4, "Square Enix"},
    {0xB6, "HAL Laboratory"},
    {0xB7, "SNK"},
    {0xB9, "Pony Canyon"},
    {0xBA, "Culture Brain"},
    {0xBB, "Sunsoft"},
    {0xBD, "Sony Imagesoft"},
    {0xBF, "Sammy Corporation"},
    {0xC0, "Taito"},
    {0xC2, "Kemco"},
    {0xC3, "Square"},
    {0xC4, "Tokuma Shoten"},
    {0xC5, "Data East"},
    {0xC6, "Tonkin House"},
    {0xC8, "Koei"},
    {0xC9, "UFL"},
    {0xCA, "Ultra Games"},
    {0xCB, "VAP, Inc."},
    {0xCC, "Use Corporation"},
    {0xCD, "Meldac"},
    {0xCE, "Pony Canyon"},
    {0xCF, "Angel"},
    {0xD0, "Taito"},
    {0xD1, "SOFEL (Software Engineering Lab)"},
    {0xD2, "Quest"},
    {0xD3, "Sigma Enterprises"},
    {0xD4, "ASK Kodansha Co."},
    {0xD6, "Naxat Soft16"},
    {0xD7, "Copya System"},
    {0xD9, "Banpresto"},
    {0xDA, "Tomy"},
    {0xDB, "LJN"},
    {0xDD, "Nippon Computer Systems"},
    {0xDE, "Human Ent."},
    {0xDF, "Altron"},
    {0xE0, "Jaleco"},
    {0xE1, "Towa Chiki"},
    {0xE2, "Yutaka # Needs more info"},
    {0xE3, "Varie"},
    {0xE5, "Epoch"},
    {0xE7, "Athena"},
    {0xE8, "Asmik Ace Entertainment"},
    {0xE9, "Natsume"},
    {0xEA, "King Records"},
    {0xEB, "Atlus"},
    {0xEC, "Epic/Sony Records"},
    {0xEE, "IGS"},
    {0xF0, "A Wave"},
    {0xF3, "Extreme Entertainment"},
    {0xFF, "LJN"},
};

const std::string getCartLicenseName() {
  std::string res = "Not found";
  if (ctx.header->license_code != 0x33) {
    res = LICENSE_CODES.at(ctx.header->license_code);
  }
  return res;
}

const std::string getROMType() { return ROM_TYPES.at(ctx.header->type); }

bool cart_load(const std::string &card_name) {
  std::ifstream file(card_name, std::ifstream::binary);

  if (file.bad()) {
    fmt::println("Failure to open a file");
    return false;
  }

  fmt::println("File {} successfully opened!", card_name);

  file.seekg(0,file.end);
  ctx.rom_size = file.tellg();
  file.seekg(0,file.beg);

  if (ctx.rom_data) {
    delete[] ctx.rom_data;
  }
  ctx.rom_data = new uint8_t[ctx.rom_size];
  file.read((char*)ctx.rom_data, ctx.rom_size);

  ctx.header = (rom_header *)(ctx.rom_data + 0x100);
  ctx.header->title[15] = 0;

  fmt::println("Cartridge Loaded:");
  fmt::println("\t Title    : {}", ctx.header->title);
  fmt::println("\t Type     : {0:x} ({1})", ctx.header->type, getROMType());
  fmt::println("\t ROM Size : {} KB", 32 << ctx.header->rom_size);
  fmt::println("\t RAM Size : {0:x}", ctx.header->ram_size);
  fmt::println("\t LIC Code : {0:x} ({1})", ctx.header->license_code,
               getCartLicenseName());
  fmt::println("\t ROM Vers : {0:x}", ctx.header->version);

  uint16_t x = 0;
  for (uint16_t i = 0x0134; i <= 0x014C; i++) {
    x = x - ctx.rom_data[i] - 1;
  }

  fmt::println("\t Checksum : {0:x} ({1})", ctx.header->checksum,
               (x & 0xFF) ? "PASSED" : "FAILED");

  return true;
}

uint8_t cart_read(uint16_t address){
  // ROM ONLY
  return ctx.rom_data[address];
}
void cart_write(uint16_t address, uint8_t value){

  NO_IMPL
}