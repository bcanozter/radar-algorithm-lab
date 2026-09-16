#pragma once

#include "../core/ConfigPort.h"
#include "../core/DataPort.h"
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
