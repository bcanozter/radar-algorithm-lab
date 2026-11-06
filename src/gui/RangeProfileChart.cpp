#include "RangeProfileChart.h"

#include "../core/utils.h"

#include <QChartView>
#include <QPainter>
#include <QVBoxLayout>

// Reference:
//  https://dev.ti.com/gallery/view/mmwave/mmWave_Demo_Visualizer/ver/3.6.0/

RangeProfileChart::RangeProfileChart(QWidget *parent) : QWidget(parent) {
  series_ = new QLineSeries();
  series_->setName(tr("Range Profile for zero Doppler"));

  chart_ = new QChart();
  chart_->addSeries(series_);
  chart_->legend()->hide();
  chart_->setAnimationOptions(QChart::NoAnimation);
  chart_->setTitle(tr("Range Profile for zero Doppler"));

  axisX_ = new QValueAxis();
  axisX_->setTitleText(tr("Range (m)"));
  axisX_->setLabelFormat("%.1f");
  chart_->addAxis(axisX_, Qt::AlignBottom);
  series_->attachAxis(axisX_);

  axisY_ = new QValueAxis();
  axisY_->setTitleText(tr("Relative Power (dB)"));
  chart_->addAxis(axisY_, Qt::AlignLeft);
  series_->attachAxis(axisY_);

  auto *chartView = new QChartView(chart_, this);
  // looks rough without aa
  chartView->setRenderHint(QPainter::Antialiasing);

  auto *layout = new QVBoxLayout(this);
  layout->setContentsMargins(0, 0, 0, 0);
  layout->addWidget(chartView);
}

void RangeProfileChart::setMaxRange(double maxRangeMeters) {
  if (maxRangeMeters <= 0.0) {
    return;
  }
  maxRangeMeters_ = maxRangeMeters;
}

void RangeProfileChart::setAntennaCount(int numRxAntennas, int numTxAntennas) {
  if (numRxAntennas <= 0 || numTxAntennas <= 0) {
    return;
  }
  numRxAntennas_ = numRxAntennas;
  numTxAntennas_ = numTxAntennas;
}

void RangeProfileChart::updateProfile(
    const std::vector<uint16_t> &rangeProfile) {
  if (rangeProfile.empty()) {
    return;
  }

  const double rangeResolution = maxRangeMeters_ / rangeProfile.size();

  // P.log2linScale[idx] = (1/256) * (2^ceil(log2(NumVirtAnt)) / NumVirtAnt);
  const int numVirtAnt = numRxAntennas_ * numTxAntennas_;
  const int numRangeBins = static_cast<int>(rangeProfile.size());

  QList<QPointF> points;
  points.reserve(rangeProfile.size());

  double minDb = 0;
  double maxDb = 0;
  for (size_t binIdx = 0; binIdx < rangeProfile.size(); ++binIdx) {
    const double range = binIdx * rangeResolution;
    const double db = log2MagnitudeToDb(rangeProfile[binIdx],
                                        numVirtAnt, numRangeBins);
    points.append(QPointF(range, db));
    minDb = std::min(minDb, db);
    maxDb = std::max(maxDb, db);
  }

  series_->replace(points);

  axisX_->setRange(0.0, (rangeProfile.size() - 1) * rangeResolution);
  const double margin = std::max(1.0, (maxDb - minDb) * 0.1);
  axisY_->setRange(minDb - margin, maxDb + margin);
}
