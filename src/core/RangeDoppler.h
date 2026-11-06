#pragma once

#include <cstdint>
#include <vector>

struct RangeDoppler
{
  int numRangeBins = 0;
  int numDopplerBins = 0;
  std::vector<float> db; // [rangeBin * numDopplerBins + dopplerBin]
};

RangeDoppler makeRangeDoppler(const std::vector<uint16_t> &heatMap,
                              int numRangeBins, int numDopplerBins,
                              int numVirtualAntennas);
