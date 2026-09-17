#pragma once

#include <array>
#include <cstdint>
#include <vector>

// TI mmWave SDK 3.6.2
// mmw_output.h

// little-endian
// header.magicWord[0] = 0x0102;
// header.magicWord[1] = 0x0304;
// header.magicWord[2] = 0x0506;
// header.magicWord[3] = 0x0708;
static constexpr std::array<uint8_t, 8> kMagicWord = {0x02, 0x01, 0x04, 0x03,
                                                       0x06, 0x05, 0x08, 0x07};

enum class MmwDemo_output_message_type : uint32_t {
    MMWDEMO_OUTPUT_MSG_DETECTED_POINTS = 1,
    MMWDEMO_OUTPUT_MSG_RANGE_PROFILE,
    MMWDEMO_OUTPUT_MSG_NOISE_PROFILE,
    MMWDEMO_OUTPUT_MSG_AZIMUT_STATIC_HEAT_MAP,
    MMWDEMO_OUTPUT_MSG_RANGE_DOPPLER_HEAT_MAP,
    MMWDEMO_OUTPUT_MSG_STATS,
    MMWDEMO_OUTPUT_MSG_DETECTED_POINTS_SIDE_INFO,
    MMWDEMO_OUTPUT_MSG_AZIMUT_ELEVATION_STATIC_HEAT_MAP,
    MMWDEMO_OUTPUT_MSG_TEMPERATURE_STATS,
    MMWDEMO_OUTPUT_MSG_MAX
};

#pragma pack(push, 1)

struct MmwDemo_output_message_header {
    uint16_t magicWord[4];
    uint32_t version;
    uint32_t totalPacketLen;
    uint32_t platform;
    uint32_t frameNumber;
    uint32_t timeCpuCycles;
    uint32_t numDetectedObj;
    uint32_t numTLVs;
    uint32_t subFrameNumber;
};

struct MmwDemo_output_message_tl {
    uint32_t type;
    uint32_t length;
};

struct DetectedPoint {
    float x;
    float y;
    float z;
    float velocity;
};

struct PointSideInfo {
    int16_t snr;
    int16_t noise;
};

struct MmwDemo_output_message_stats {
    uint32_t interFrameProcessingTime;
    uint32_t transmitOutputTime;
    uint32_t interFrameProcessingMargin;
    uint32_t interChirpProcessingMargin;
    uint32_t activeFrameCPULoad;
    uint32_t interFrameCPULoad;
};

#pragma pack(pop)

struct Frame {
    MmwDemo_output_message_header header{};
    std::vector<DetectedPoint> points;
    std::vector<PointSideInfo> pointSideInfo;
    std::vector<uint16_t> rangeProfile;
    std::vector<uint16_t> noiseProfile;
    MmwDemo_output_message_stats stats{};

    std::vector<uint8_t> rawBytes;
};