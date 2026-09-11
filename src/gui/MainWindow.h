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

private:
  ConfigPanel *configPanel_;
  LogView *logView_;
};
