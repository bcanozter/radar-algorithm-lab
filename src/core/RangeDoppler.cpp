#include "RangeDoppler.h"

#include "utils.h"
#include <cstring>

RangeDoppler makeRangeDoppler(const std::vector<uint16_t> &heatMap,
                              int numRangeBins, int numDopplerBins,
                              int numVirtualAntennas)
{
  RangeDoppler map;
  std::memset(&map, 0, sizeof(RangeDoppler));

  if (heatMap.size() !=
      static_cast<size_t>(numRangeBins) * numDopplerBins)
  {
    return map;
  }
  map.numRangeBins = numRangeBins;
  map.numDopplerBins = numDopplerBins;
  map.db.resize(heatMap.size());

  const int half = numDopplerBins / 2;
  for (int r = 0; r < numRangeBins; ++r)
  {
    const int row = r * numDopplerBins;
    for (int d = 0; d < numDopplerBins; ++d)
    {
      const int shifted = (d < half) ? d + half : d - half;
      map.db[row + shifted] = static_cast<float>(log2MagnitudeToDb(
          heatMap[row + d], numVirtualAntennas, numRangeBins));
    }
  }
  return map;
}
