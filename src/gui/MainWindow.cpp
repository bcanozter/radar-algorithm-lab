#include "MainWindow.h"

#include <QAction>
#include <QDebug>
#include <QSplitter>
#include <QToolBar>

#include <bitset>

#include "../core/SerialPort.h"
#include "ConfigPanel.h"
#include "ConnectDialog.h"
#include "DeviceStatsPanel.h"
#include "LogView.h"
#include "RangeProfileChart.h"
#include "ScatterPlot2D.h"

MainWindow::MainWindow(QWidget *parent) : QMainWindow(parent) {
  setWindowTitle(tr("Radar Algorithm Lab"));
  resize(1500, 820);
  configPanel_ = new ConfigPanel(this);
  logView_ = new LogView(this);
  configPort_.setLogView(logView_);

  rangeProfileChart_ = new RangeProfileChart(this);
  scatterPlot2D_ = new ScatterPlot2D(this);
  deviceStatsPanel_ = new DeviceStatsPanel(this);
  // test
  // const auto ports = getAvailablePorts();

  auto *configSplitter = new QSplitter(Qt::Horizontal, this);
  configSplitter->addWidget(configPanel_);
  configSplitter->addWidget(logView_);
  configSplitter->setStretchFactor(0, 3);
  configSplitter->setStretchFactor(1, 2);

  auto *visualizeSplitter = new QSplitter(Qt::Horizontal, this);
  visualizeSplitter->addWidget(rangeProfileChart_);
  visualizeSplitter->addWidget(scatterPlot2D_);
  visualizeSplitter->setStretchFactor(0, 1);
  visualizeSplitter->setStretchFactor(1, 1);
  visualizeSplitter->setSizes({750, 750});

  auto *plotsSplitter = new QSplitter(Qt::Vertical, this);
  plotsSplitter->addWidget(visualizeSplitter);
  plotsSplitter->addWidget(deviceStatsPanel_);
  plotsSplitter->setStretchFactor(0, 4);
  plotsSplitter->setStretchFactor(1, 1);
  plotsSplitter->setSizes({600, 180});

  auto *tabs = new QTabWidget(this);
  tabs->addTab(configSplitter, tr("Config"));
  tabs->addTab(plotsSplitter, tr("Plots"));
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
  updateRadarConfigFromCfg_(lines);
  configPort_.sendConfigFile(lines);
}

void MainWindow::onStartSensorRequested() {
  configPort_.sendCommand("sensorStart 0");
}

void MainWindow::onStopSensorRequested() {
  configPort_.sendCommand("sensorStop");
}

void MainWindow::handleIncomingFrame(const Frame &frame) {
  rangeProfileChart_->updateProfile(frame.rangeProfile);
  scatterPlot2D_->updatePoints(frame.points);
  deviceStatsPanel_->updateFrame(frame);
}

// mmwave sdk user guide explains these .cfg parameters
void MainWindow::updateRadarConfigFromCfg_(const QStringList &lines) {
  for (const QString &raw : lines) {
    const QString line = raw.trimmed();

    if (line.startsWith(QLatin1String("profileCfg"))) {
      const QStringList tokens = line.split(' ', Qt::SkipEmptyParts);
      if (tokens.size() < 12) {
        continue;
      }

      const double freqSlopeConst = tokens[8].toDouble(); // MHz/us
      const int numAdcSamples = tokens[10].toInt();
      const double digOutSampleRate = tokens[11].toDouble(); // ksps
      if (freqSlopeConst <= 0.0 || numAdcSamples <= 0 ||
          digOutSampleRate <= 0.0) {
        continue;
      }

      constexpr double kSpeedOfLight = 3.0e8; // m/s
      const double maxRange = (kSpeedOfLight * digOutSampleRate * 1e3) /
                              (2.0 * freqSlopeConst * 1e12);
      rangeProfileChart_->setMaxRange(maxRange);
      scatterPlot2D_->setMaxRange(maxRange);
    } else if (line.startsWith(QLatin1String("channelCfg"))) {
      const QStringList tokens = line.split(' ', Qt::SkipEmptyParts);
      if (tokens.size() < 3) {
        continue;
      }

      const uint32_t rxChannelEn = tokens[1].toUInt(nullptr, 0);
      const uint32_t txChannelEn = tokens[2].toUInt(nullptr, 0);
      // 0x1111b = 15 -> 4 antennas
      const int numRxAntennas = std::bitset<4>(rxChannelEn).count();
      const int numTxAntennas = std::bitset<4>(txChannelEn).count();
      rangeProfileChart_->setAntennaCount(numRxAntennas, numTxAntennas);
    }
  }
}
