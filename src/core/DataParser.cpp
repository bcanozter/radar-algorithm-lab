#include "DataParser.h"
#include "TlvTypes.h"

#include <QDebug>
#include <algorithm>
#include <cstdint>
#include <cstring>

size_t findMmwaveMagicWord(const uint8_t *data, size_t len) {
  if (len < kMagicWord.size()) {
    return len;
  }
  const size_t last = len - kMagicWord.size();
  for (size_t i = 0; i <= last; ++i) {
    if (std::equal(kMagicWord.begin(), kMagicWord.end(), data + i)) {
      return i;
    }
  }
  return len;
}

bool DataParser::decodeFrame(const uint8_t *data, uint32_t len,
                             Frame &outFrame) {
  MmwDemo_output_message_header header;
  std::memcpy(&header, data, sizeof(MmwDemo_output_message_header));
  outFrame.header = header;
  outFrame.rawBytes.assign(data, data + len);

  uint32_t offset = sizeof(MmwDemo_output_message_header);

  for (uint32_t tlvIdx = 0; tlvIdx < header.numTLVs; ++tlvIdx) {
    if (len - offset < sizeof(MmwDemo_output_message_tl)) {
      return false;
    }
    MmwDemo_output_message_tl tlv;
    std::memcpy(&tlv, data + offset, sizeof(MmwDemo_output_message_tl));
    offset += sizeof(MmwDemo_output_message_tl);

    if (tlv.length > len - offset) {
      return false;
    }
    const uint8_t *payload = data + offset;

    switch (static_cast<MmwDemo_output_message_type>(tlv.type)) {
    case MmwDemo_output_message_type::MMWDEMO_OUTPUT_MSG_DETECTED_POINTS: {
      const size_t n = tlv.length / sizeof(DetectedPoint);
      outFrame.points.resize(n);
      std::memcpy(outFrame.points.data(), payload, n * sizeof(DetectedPoint));
      break;
    }
    case MmwDemo_output_message_type::
        MMWDEMO_OUTPUT_MSG_DETECTED_POINTS_SIDE_INFO: {
      const size_t n = tlv.length / sizeof(PointSideInfo);
      outFrame.pointSideInfo.resize(n);
      std::memcpy(outFrame.pointSideInfo.data(), payload,
                  n * sizeof(PointSideInfo));
      break;
    }
    case MmwDemo_output_message_type::MMWDEMO_OUTPUT_MSG_RANGE_PROFILE: {
      const size_t n = tlv.length / sizeof(uint16_t);
      outFrame.rangeProfile.resize(n);
      std::memcpy(outFrame.rangeProfile.data(), payload, n * sizeof(uint16_t));
      break;
    }
    case MmwDemo_output_message_type::MMWDEMO_OUTPUT_MSG_NOISE_PROFILE: {
      const size_t n = tlv.length / sizeof(uint16_t);
      outFrame.noiseProfile.resize(n);
      std::memcpy(outFrame.noiseProfile.data(), payload, n * sizeof(uint16_t));
      break;
    }
    case MmwDemo_output_message_type::MMWDEMO_OUTPUT_MSG_STATS: {
      if (tlv.length >= sizeof(MmwDemo_output_message_stats)) {
        std::memcpy(&outFrame.stats, payload,
                    sizeof(MmwDemo_output_message_stats));
      }
      break;
    }
    default:
      qDebug() << "Unknown TLV Type (" << tlv.type << ")";
      break;
    }

    offset += tlv.length;
  }

  return true;
}

size_t DataParser::nextPacketLength(std::vector<uint8_t> &buffer) {
  for (;;) {
    const size_t magicPos = findMmwaveMagicWord(buffer.data(), buffer.size());

    if (magicPos == buffer.size()) {

      const size_t keep = std::min(buffer.size(), kMagicWord.size() - 1);
      buffer.erase(buffer.begin(), buffer.end() - static_cast<long>(keep));
      return 0;
    }

    buffer.erase(buffer.begin(), buffer.begin() + static_cast<long>(magicPos));

    if (buffer.size() < sizeof(MmwDemo_output_message_header)) {
      return 0;
    }

    MmwDemo_output_message_header header;
    std::memcpy(&header, buffer.data(), sizeof(MmwDemo_output_message_header));
    const uint32_t totalPacketLen = header.totalPacketLen;

    if (totalPacketLen < sizeof(MmwDemo_output_message_header) ||
        totalPacketLen > kMaxPacketLen) {
      buffer.erase(buffer.begin(), buffer.begin() + 1);
      continue;
    }

    if (buffer.size() < totalPacketLen) {
      return 0;
    }

    return totalPacketLen;
  }
}

std::vector<Frame> DataParser::parse(std::vector<uint8_t> &buffer) {
  std::vector<Frame> frames;

  while (const size_t len = nextPacketLength(buffer)) {
    Frame frame;
    if (decodeFrame(buffer.data(), static_cast<uint32_t>(len), frame)) {
      frames.push_back(std::move(frame));
    }
    buffer.erase(buffer.begin(), buffer.begin() + static_cast<long>(len));
  }

  return frames;
}
