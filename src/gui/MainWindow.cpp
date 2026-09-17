#include "MainWindow.h"

#include <QAction>
#include <QDebug>
#include <QSplitter>
#include <QToolBar>

#include "../core/SerialPort.h"
#include "ConfigPanel.h"
#include "ConnectDialog.h"
#include "LogView.h"

MainWindow::MainWindow(QWidget *parent) : QMainWindow(parent) {
  setWindowTitle(tr("Radar Algorithm Lab"));
  resize(1500, 820);
  configPanel_ = new ConfigPanel(this);
  logView_ = new LogView(this);
  configPort_.setLogView(logView_);

  // test
  // const auto ports = getAvailablePorts();

  auto *configSplitter = new QSplitter(Qt::Horizontal, this);
  configSplitter->addWidget(configPanel_);
  configSplitter->addWidget(logView_);
  configSplitter->setStretchFactor(0, 3);
  configSplitter->setStretchFactor(1, 2);

  auto *tabs = new QTabWidget(this);
  tabs->addTab(configSplitter, tr("Config"));
  setCentralWidget(tabs);

  connectAction_ = new QAction(tr("Connect"), this);
  disconnectAction_ = new QAction(tr("Disconnect"), this);
  disconnectAction_->setEnabled(false);
  connect(connectAction_, &QAction::triggered, this,
          &MainWindow::handleConnect_);
  connect(disconnectAction_, &QAction::triggered, this,
          &MainWindow::handleDisconnect_);

  QToolBar *toolbar = addToolBar(tr("Connection"));
  toolbar->addAction(connectAction_);
  toolbar->addAction(disconnectAction_);

  connect(configPanel_, &ConfigPanel::sendConfigRequested, this,
          &MainWindow::onSendConfigRequested);
  connect(configPanel_, &ConfigPanel::startSensorRequested, this,
          &MainWindow::onStartSensorRequested);
  connect(configPanel_, &ConfigPanel::stopSensorRequested, this,
          &MainWindow::onStopSensorRequested);

  // incoming data

  connect(&dataPort_, &DataPort::frameReceived, this,
          &MainWindow::handleIncomingFrame);
}

void MainWindow::handleConnect_() {
  ConnectDialog dialog(this);
  if (dialog.exec() != QDialog::Accepted) {
    return;
  }

  const QString newConfigPortAddr = dialog.getConfigPortAddress();
  const QString newDataPortAddr = dialog.getDataPortAddress();
  if (newConfigPortAddr.isEmpty() || newDataPortAddr.isEmpty()) {
    return;
  }
  if (newConfigPortAddr == newDataPortAddr) {
    qWarning() << "Config Port and Data Port cannot be the same";
    return;
  }

  QString error;
  if (!configPort_.connectPort(newConfigPortAddr,
                               dialog.getConfigPortBaudRate(), error)) {
    qWarning() << "Unable to connect to config port(" << newConfigPortAddr
               << ")";
    return;
  }
  if (!dataPort_.connectPort(newDataPortAddr, dialog.getDataPortBaudRate(),
                             error)) {

    qWarning() << "Unable to connect to data port(" << newDataPortAddr << ")";
    configPort_.disconnectPort();
    return;
  }

  currentConfigPortAddr = newConfigPortAddr;
  currentDataPortAddr = newDataPortAddr;

  connectAction_->setEnabled(false);
  disconnectAction_->setEnabled(true);
}

void MainWindow::handleDisconnect_() {
  dataPort_.disconnectPort();
  configPort_.disconnectPort();
  connectAction_->setEnabled(true);
  disconnectAction_->setEnabled(false);
}

void MainWindow::onSendConfigRequested(const QStringList &lines) {
  configPort_.sendConfigFile(lines);
}

void MainWindow::onStartSensorRequested() {
  configPort_.sendCommand("sensorStart 0");
}

void MainWindow::onStopSensorRequested() {
  configPort_.sendCommand("sensorStop");
}

void MainWindow::handleIncomingFrame(const Frame &frame) {
  // todo
}
