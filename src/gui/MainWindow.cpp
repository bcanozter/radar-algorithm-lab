#include "MainWindow.h"

#include <QAction>
#include <QDateTime>
#include <QDebug>
#include <QFileDialog>
#include <QFileInfo>
#include <QLabel>
#include <QSplitter>
#include <QStatusBar>
#include <QToolBar>

#include <bitset>

#include "../core/SerialPort.h"
#include "ConfigPanel.h"
#include "ConnectDialog.h"
#include "DeviceStatsPanel.h"
#include "LogView.h"
#include "RangeProfileChart.h"
#include "ReplayBar.h"
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

  recordAction_ = new QAction(tr("Record"), this);
  recordAction_->setCheckable(true);
  recordAction_->setEnabled(false);
  loadRecordingAction_ = new QAction(tr("Load Recording"), this);
  connect(recordAction_, &QAction::triggered, this, &MainWindow::handleRecord_);
  connect(loadRecordingAction_, &QAction::triggered, this,
          &MainWindow::handleLoadRecording_);

  QToolBar *toolbar = addToolBar(tr("Connection"));
  toolbar->addAction(connectAction_);
  toolbar->addAction(disconnectAction_);
  toolbar->addSeparator();
  toolbar->addAction(recordAction_);
  toolbar->addAction(loadRecordingAction_);

  QToolBar *replayToolbar = addToolBar(tr("Replay"));
  replayToolbar->addWidget(new ReplayBar(&recording_, this));

  recordingLabel_ = new QLabel(this);
  recordingLabel_->setStyleSheet("color: red; font-weight: bold;");
  recordingLabel_->hide();
  statusBar()->addPermanentWidget(recordingLabel_);
  recordingStatusTimer_.setInterval(1000);
  connect(&recordingStatusTimer_, &QTimer::timeout, this,
          &MainWindow::updateRecordingStatus_);

  connect(configPanel_, &ConfigPanel::sendConfigRequested, this,
          &MainWindow::onSendConfigRequested);
  connect(configPanel_, &ConfigPanel::startSensorRequested, this,
          &MainWindow::onStartSensorRequested);
  connect(configPanel_, &ConfigPanel::stopSensorRequested, this,
          &MainWindow::onStopSensorRequested);

  // incoming data

  connect(&dataPort_, &DataPort::frameReceived, this,
          &MainWindow::handleIncomingFrame);
  connect(&dataPort_, &DataPort::frameReceived, &recording_,
          &Recording::writeFrame);
  connect(&recording_, &Recording::frameReceived, this,
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

  // live data and replay both feed the same plots
  recording_.unload();

  connectAction_->setEnabled(false);
  disconnectAction_->setEnabled(true);
  recordAction_->setEnabled(true);
  loadRecordingAction_->setEnabled(false);
}

void MainWindow::handleDisconnect_() {
  recording_.stopRecording();
  dataPort_.disconnectPort();
  configPort_.disconnectPort();
  connectAction_->setEnabled(true);
  disconnectAction_->setEnabled(false);
  recordAction_->setEnabled(false);
  loadRecordingAction_->setEnabled(true);
  updateRecordingUi_();
}

void MainWindow::handleRecord_(bool checked) {
  if (checked) {
    startRecording_();
  } else {
    recording_.stopRecording();
    qInfo() << "Recording stopped";
  }
  updateRecordingUi_();
}

void MainWindow::startRecording_() {
  const QString defaultName =
      "recording_" + QDateTime::currentDateTime().toString("yyyyMMdd_HHmmss") +
      ".bin";
  QString path = QFileDialog::getSaveFileName(
      this, tr("Save Recording"), defaultName, tr("Recordings (*.bin)"));
  if (path.isEmpty()) {
    return;
  }
  if (!path.endsWith(".bin")) {
    path += ".bin";
  }

  QString error;
  if (!recording_.startRecording(path, lastCfgLines_, error)) {
    qWarning() << "Unable to start recording:" << error;
    return;
  }
  qInfo() << "Recording to" << path;
}

// Keep the Record button and the status bar in sync with the recording.
void MainWindow::updateRecordingUi_() {
  const bool recording = recording_.isRecording();
  recordAction_->setChecked(recording);
  recordAction_->setText(recording ? tr("Stop Recording") : tr("Record"));
  recordingLabel_->setVisible(recording);
  if (recording) {
    updateRecordingStatus_();
    recordingStatusTimer_.start();
  } else {
    recordingStatusTimer_.stop();
  }
}

void MainWindow::updateRecordingStatus_() {
  const qint64 seconds = recording_.elapsedMs() / 1000;
  const double megabytes = recording_.bytesWritten() / (1024.0 * 1024.0);
  recordingLabel_->setText(QString("* REC  %1:%2  ·  %3 MB  ·  %4")
                               .arg(seconds / 60, 2, 10, QChar('0'))
                               .arg(seconds % 60, 2, 10, QChar('0'))
                               .arg(megabytes, 0, 'f', 1)
                               .arg(QFileInfo(recording_.path()).fileName()));
}

void MainWindow::handleLoadRecording_() {
  const QString path = QFileDialog::getOpenFileName(
      this, tr("Load Recording"), QString(), tr("Recordings (*.bin)"));
  if (path.isEmpty()) {
    return;
  }

  QString error;
  if (!recording_.load(path, error)) {
    qWarning() << "Unable to load recording:" << error;
    return;
  }
  if (recording_.cfgLines().isEmpty()) {
    qWarning() << "No .cfg found next to the recording, plot ranges may be off";
  } else {
    updateRadarConfigFromCfg_(recording_.cfgLines());
  }
  qInfo() << "Loaded recording" << path << "with" << recording_.frameCount()
          << "frames";
  recording_.seek(0);
}

void MainWindow::onSendConfigRequested(const QStringList &lines) {
  lastCfgLines_ = lines;
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
