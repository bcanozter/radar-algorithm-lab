#pragma once

#include "../core/ConfigPort.h"
#include "../core/DataParser.h"
#include "../core/DataPort.h"
#include "../core/Recording.h"
#include <QMainWindow>
#include <QTimer>

class QLabel;
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
  void handleRecord_(bool checked);
  void handleLoadRecording_();

private:
  void updateRadarConfigFromCfg_(const QStringList &lines);
  void startRecording_();
  void updateRecordingUi_();
  void updateRecordingStatus_();

  ConfigPanel *configPanel_;
  LogView *logView_;
  RangeProfileChart *rangeProfileChart_;
  ScatterPlot2D *scatterPlot2D_;
  DeviceStatsPanel *deviceStatsPanel_;

  QAction *connectAction_;
  QAction *disconnectAction_;
  QAction *recordAction_;
  QAction *loadRecordingAction_;
  QLabel *recordingLabel_;
  QTimer recordingStatusTimer_;

  ConfigPort configPort_;
  DataPort dataPort_;
  Recording recording_;

  QString currentConfigPortAddr;
  QString currentDataPortAddr;
  // last cfg sent to the device
  QStringList lastCfgLines_;
};
