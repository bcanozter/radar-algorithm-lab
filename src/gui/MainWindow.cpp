#include "MainWindow.h"

#include <QDebug>
#include <QSplitter>

#include "../core/SerialPort.h"
#include "ConfigPanel.h"
#include "LogView.h"

MainWindow::MainWindow(QWidget *parent) : QMainWindow(parent) {
  setWindowTitle(tr("Radar Algorithm Lab"));
  resize(1500, 820);
  configPanel_ = new ConfigPanel(this);
  logView_ = new LogView(this);

  // test
  const auto ports = getAvailablePorts();

  auto *configSplitter = new QSplitter(Qt::Horizontal, this);
  configSplitter->addWidget(configPanel_);
  configSplitter->addWidget(logView_);
  configSplitter->setStretchFactor(0, 3);
  configSplitter->setStretchFactor(1, 2);

  auto *tabs = new QTabWidget(this);
  tabs->addTab(configSplitter, tr("Config"));
  setCentralWidget(tabs);
}
