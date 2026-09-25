#include "ScatterPlot2D.h"

#include <QChart>
#include <QChartView>
#include <QCursor>
#include <QPainter>
#include <QScatterSeries>
#include <QToolTip>
#include <QVBoxLayout>
#include <QValueAxis>

// 2D Scatter
ScatterPlot2D::ScatterPlot2D(QWidget *parent) : QWidget(parent) {
  series_ = new QScatterSeries();
  series_->setName(tr("Detected Objects"));
  series_->setMarkerShape(QScatterSeries::MarkerShapeCircle);
  series_->setMarkerSize(10.0);
  series_->setColor(QColor(64, 200, 140));
  series_->setBorderColor(Qt::transparent);

  chart_ = new QChart();
  chart_->addSeries(series_);
  chart_->legend()->hide();
  chart_->setAnimationOptions(QChart::NoAnimation);
  chart_->setTitle(tr("Detected Objects (Top View)"));

  axisX_ = new QValueAxis();
  axisX_->setTitleText(tr("X (m)"));
  axisX_->setLabelFormat("%.1f");
  chart_->addAxis(axisX_, Qt::AlignBottom);
  series_->attachAxis(axisX_);

  axisY_ = new QValueAxis();
  axisY_->setTitleText(tr("Y (m)"));
  axisY_->setLabelFormat("%.1f");
  chart_->addAxis(axisY_, Qt::AlignLeft);
  series_->attachAxis(axisY_);

  applyAxisRanges_();

  connect(series_, &QScatterSeries::hovered, this,
          &ScatterPlot2D::onPointHovered);

  auto *chartView = new QChartView(chart_, this);
  chartView->setRenderHint(QPainter::Antialiasing);

  auto *layout = new QVBoxLayout(this);
  layout->setContentsMargins(0, 0, 0, 0);
  layout->addWidget(chartView);
}

void ScatterPlot2D::setMaxRange(double maxRangeMeters) {
  if (maxRangeMeters <= 0.0) {
    return;
  }
  maxRangeMeters_ = maxRangeMeters;
  applyAxisRanges_();
}

void ScatterPlot2D::applyAxisRanges_() {
  axisX_->setRange(-maxRangeMeters_ / 2.0, maxRangeMeters_ / 2.0);
  axisY_->setRange(0.0, maxRangeMeters_);
}

void ScatterPlot2D::updatePoints(const std::vector<DetectedPoint> &points) {
  lastPoints_ = points;
  chartPoints_.clear();
  chartPoints_.reserve(qsizetype(points.size()));
  for (const auto &point : points) {
    chartPoints_.append(QPointF(point.x, point.y));
  }
  series_->replace(chartPoints_);
}

void ScatterPlot2D::onPointHovered(const QPointF &point, bool state) {
  if (!state) {
    QToolTip::hideText();
    return;
  }
  const qsizetype index = chartPoints_.indexOf(point);
  if (index < 0 || size_t(index) >= lastPoints_.size()) {
    return;
  }
  const DetectedPoint &p = lastPoints_[size_t(index)];
  const QString text = tr("X: %1 m\nY: %2 m\nZ: %3 m\nVelocity: %4 m/s")
                           .arg(p.x, 0, 'f', 2)
                           .arg(p.y, 0, 'f', 2)
                           .arg(p.z, 0, 'f', 2)
                           .arg(p.velocity, 0, 'f', 2);
  QToolTip::showText(QCursor::pos(), text, this);
}
