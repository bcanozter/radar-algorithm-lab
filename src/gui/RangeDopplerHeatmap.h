#pragma once

#include <QWidget>

class RangeDopplerHeatmap : public QWidget
{
  Q_OBJECT
public:
  explicit RangeDopplerHeatmap(QWidget *parent = nullptr);

  void setMaxRange(double maxRangeMeters);
  void setRangeBins(int numRangeBins);
  void setDopplerBins(int numDopplerBins);
  void setDopplerResolution(double dopplerResolutionMps);
  void setAntennaCount(int numRxAntennas, int numTxAntennas);

private:
  double maxRangeMeters_ = 1.0;
  double dopplerResolutionMps_ = 1.0;
  int numRangeBins_ = 0;
  int numDopplerBins_ = 0;
  int numRxAntennas_ = 0;
  int numTxAntennas_ = 0;
};
