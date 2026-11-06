#include "RangeDopplerHeatmap.h"

RangeDopplerHeatmap::RangeDopplerHeatmap(QWidget *parent) : QWidget(parent) {}

void RangeDopplerHeatmap::setMaxRange(double maxRangeMeters)
{
  if (maxRangeMeters <= 0.0)
  {
    return;
  }
  maxRangeMeters_ = maxRangeMeters;
}

void RangeDopplerHeatmap::setRangeBins(int numRangeBins)
{
  numRangeBins_ = numRangeBins;
}

void RangeDopplerHeatmap::setDopplerBins(int numDopplerBins)
{
  numDopplerBins_ = numDopplerBins;
}

void RangeDopplerHeatmap::setDopplerResolution(double dopplerResolutionMps)
{
  if (dopplerResolutionMps <= 0.0)
  {
    return;
  }
  dopplerResolutionMps_ = dopplerResolutionMps;
}

void RangeDopplerHeatmap::setAntennaCount(int numRxAntennas,
                                          int numTxAntennas)
{
  numRxAntennas_ = numRxAntennas;
  numTxAntennas_ = numTxAntennas;
}
