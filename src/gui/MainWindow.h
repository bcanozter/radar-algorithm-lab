#pragma once

#include "../core/ConfigPort.h"
#include "../core/DataPort.h"
#include "../core/DataParser.h"
#include <QMainWindow>

class ConfigPanel;
class LogView;
class MainWindow : public QMainWindow {
  Q_OBJECT
public:
  explicit MainWindow(QWidget *parent = nullptr);

private slots:
  void handleConnect_();
  void handleDisconnect_();
  void onSendConfigRequested(const QStringList& lines);
  void onStartSensorRequested();
  void onStopSensorRequested();
  void handleIncomingFrame(const Frame& frame);

private:
  ConfigPanel *configPanel_;
  LogView *logView_;

  QAction *connectAction_;
  QAction *disconnectAction_;

  ConfigPort configPort_;
  DataPort dataPort_;

  QString currentConfigPortAddr;
  QString currentDataPortAddr;
};
