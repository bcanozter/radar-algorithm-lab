#pragma once

#include <QList>
#include <QPointF>
#include <QWidget>
#include <vector>

#include "../core/TlvTypes.h"

class QChart;
class QScatterSeries;
class QValueAxis;

class ScatterPlot2D : public QWidget {
  Q_OBJECT
public:
  explicit ScatterPlot2D(QWidget *parent = nullptr);

  void setMaxRange(double maxRangeMeters);

  void updatePoints(const std::vector<DetectedPoint> &points);

private slots:
  void onPointHovered(const QPointF &point, bool state);

private:
  void applyAxisRanges_();

  QChart *chart_;
  QScatterSeries *series_;
  QValueAxis *axisX_;
  QValueAxis *axisY_;
  double maxRangeMeters_ = 10.0;

  std::vector<DetectedPoint> lastPoints_;
  QList<QPointF> chartPoints_;
};
