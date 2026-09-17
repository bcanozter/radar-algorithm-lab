#pragma once

#include <cstdint>
#include <vector>

#include "TlvTypes.h"

class DataParser {
public:

    std::vector<Frame> parse(std::vector<uint8_t>& buffer);

private:
    static constexpr uint32_t kMaxPacketLen = 1u << 20; // 1 MiB

    static size_t nextPacketLength(std::vector<uint8_t>& buffer);
    static bool decodeFrame(const uint8_t* data, uint32_t len, Frame& outFrame);
};