#pragma once
#include "all.hpp"
#include "common.hpp"
#include "ppu.hpp"
#include <cstdint>

void ppu_fifo_push(uint32_t);
uint32_t ppu_fifo_pop();
void pipeline_process();
void pipeline_fifo_reset();