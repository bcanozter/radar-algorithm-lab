#include "RangeProfileChart.h"

#include <QChartView>
#include <QPainter>
#include <QVBoxLayout>

// Reference:
//  https://dev.ti.com/gallery/view/mmwave/mmWave_Demo_Visualizer/ver/3.6.0/

double dspFftScalComp2(double fftMinSize, double fftSize) {
  return fftMinSize / fftSize;
}

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

double RangeProfileChart::toRelativeDb(uint16_t raw, double log2linScale,
                                       double dspFftScaleCompAllLog) const {
  constexpr double kToDB = 20.0 * std::log10(2);
  return raw * log2linScale * kToDB + dspFftScaleCompAllLog;
}

void RangeProfileChart::updateProfile(
    const std::vector<uint16_t> &rangeProfile) {
  if (rangeProfile.empty()) {
    return;
  }

  const double rangeResolution = maxRangeMeters_ / rangeProfile.size();

  // P.log2linScale[idx] = (1/256) * (2^ceil(log2(NumVirtAnt)) / NumVirtAnt);
  const int numVirtAnt = numRxAntennas_ * numTxAntennas_;
  const double log2linScale = (1.0 / 256.0) *
                              std::pow(2, std::ceil(std::log2(numVirtAnt))) /
                              numVirtAnt;

  const double dspFftScaleComp1DLog =
      20.0 * std::log10(dspFftScalComp2(32.0, rangeProfile.size()));
  const double dspFftScaleCompAllLog = dspFftScaleComp1DLog;

  QList<QPointF> points;
  points.reserve(rangeProfile.size());

  double minDb = 0;
  double maxDb = 0;
  for (size_t binIdx = 0; binIdx < rangeProfile.size(); ++binIdx) {
    const double range = binIdx * rangeResolution;
    const double db =
        toRelativeDb(rangeProfile[binIdx], log2linScale, dspFftScaleCompAllLog);
    points.append(QPointF(range, db));
    minDb = std::min(minDb, db);
    maxDb = std::max(maxDb, db);
  }

  series_->replace(points);

  axisX_->setRange(0.0, (rangeProfile.size() - 1) * rangeResolution);
  const double margin = std::max(1.0, (maxDb - minDb) * 0.1);
  axisY_->setRange(minDb - margin, maxDb + margin);
}
