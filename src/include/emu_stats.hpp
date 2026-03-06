#pragma once


#include <cstdint>
struct CPUStats {
    uint64_t cycles;
    uint64_t instructions;
    uint64_t time_us;
};
