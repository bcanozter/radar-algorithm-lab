#pragma once

#include <QWidget>
#include <QtCharts/QChart>
#include <QtCharts/QLineSeries>
#include <QtCharts/QValueAxis>
#include <vector>

class RangeProfileChart : public QWidget {
  Q_OBJECT
public:
  explicit RangeProfileChart(QWidget *parent = nullptr);

  void setMaxRange(double maxRangeMeters);
  void setAntennaCount(int numRxAntennas, int numTxAntennas);

  void updateProfile(const std::vector<uint16_t> &rangeProfile);

private:
  double toRelativeDb(uint16_t raw, double log2linScale,
                      double dspFftScaleCompAllLog) const;

  QChart *chart_;
  QLineSeries *series_;
  QValueAxis *axisX_;
  QValueAxis *axisY_;
  double maxRangeMeters_ = 1.0;
  int numRxAntennas_ = 1;
  int numTxAntennas_ = 1;
};
