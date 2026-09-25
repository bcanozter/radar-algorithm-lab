#pragma once

#include "../core/ConfigPort.h"
#include "../core/DataParser.h"
#include "../core/DataPort.h"
#include <QMainWindow>

class ConfigPanel;
class LogView;
class RangeProfileChart;
class ScatterPlot2D;
class DeviceStatsPanel;
class MainWindow : public QMainWindow {
  Q_OBJECT
public:
  explicit MainWindow(QWidget *parent = nullptr);

private slots:
  void handleConnect_();
  void handleDisconnect_();
  void onSendConfigRequested(const QStringList &lines);
  void onStartSensorRequested();
  void onStopSensorRequested();
  void handleIncomingFrame(const Frame &frame);

private:
  void updateRadarConfigFromCfg_(const QStringList &lines);

  ConfigPanel *configPanel_;
  LogView *logView_;
  RangeProfileChart *rangeProfileChart_;
  ScatterPlot2D *scatterPlot2D_;
  DeviceStatsPanel *deviceStatsPanel_;

  QAction *connectAction_;
  QAction *disconnectAction_;

  ConfigPort configPort_;
  DataPort dataPort_;

  QString currentConfigPortAddr;
  QString currentDataPortAddr;
};
