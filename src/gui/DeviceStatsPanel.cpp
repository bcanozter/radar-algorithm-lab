#include "DeviceStatsPanel.h"

#include <QFormLayout>
#include <QGroupBox>
#include <QHBoxLayout>
#include <QLabel>
#include <QVBoxLayout>

DeviceStatsPanel::DeviceStatsPanel(QWidget *parent) : QWidget(parent) {
  auto *frameGroup = new QGroupBox(tr("Frame Stats"), this);
  auto *frameForm = new QFormLayout(frameGroup);
  addStatRow_(frameForm, tr("Frame #"), "frameNumber");
  addStatRow_(frameForm, tr("Detected Objects"), "detectedObjects");
  addStatRow_(frameForm, tr("Frame Time"), "frameTime");
  addStatRow_(frameForm, tr("Inter-Frame Processing Time"),
              "interFrameProcTime");
  addStatRow_(frameForm, tr("Transmit Output Time"), "transmitOutputTime");
  addStatRow_(frameForm, tr("Inter-Frame Processing Margin"),
              "interFrameMargin");
  addStatRow_(frameForm, tr("Inter-Chirp Processing Margin"),
              "interChirpMargin");
  addStatRow_(frameForm, tr("Active Frame CPU Load"), "activeFrameCpuLoad");
  addStatRow_(frameForm, tr("Inter-Frame CPU Load"), "interFrameCpuLoad");

  auto *tempGroup = new QGroupBox(tr("RF Temperature (deg C)"), this);
  auto *tempOuter = new QVBoxLayout(tempGroup);
  auto *tempStatusForm = new QFormLayout();
  addStatRow_(tempStatusForm, tr("Status"), "tempStatus");
  tempOuter->addLayout(tempStatusForm);

  auto *tempReadings = new QHBoxLayout();
  auto *tempFormLeft = new QFormLayout();
  addStatRow_(tempFormLeft, tr("Rx0"), "tempRx0");
  addStatRow_(tempFormLeft, tr("Rx1"), "tempRx1");
  addStatRow_(tempFormLeft, tr("Rx2"), "tempRx2");
  addStatRow_(tempFormLeft, tr("Rx3"), "tempRx3");
  addStatRow_(tempFormLeft, tr("PM"), "tempPm");
  tempReadings->addLayout(tempFormLeft);

  auto *tempFormRight = new QFormLayout();
  addStatRow_(tempFormRight, tr("Tx0"), "tempTx0");
  addStatRow_(tempFormRight, tr("Tx1"), "tempTx1");
  addStatRow_(tempFormRight, tr("Tx2"), "tempTx2");
  addStatRow_(tempFormRight, tr("Dig0"), "tempDig0");
  addStatRow_(tempFormRight, tr("Dig1"), "tempDig1");
  tempReadings->addLayout(tempFormRight);

  tempOuter->addLayout(tempReadings);
  tempOuter->addStretch();

  auto *layout = new QHBoxLayout(this);
  layout->addWidget(frameGroup);
  layout->addWidget(tempGroup);
}

QLabel *DeviceStatsPanel::addStatRow_(QFormLayout *form,
                                      const QString &labelText,
                                      const QString &key) {
  auto *value = new QLabel(tr("--"));
  form->addRow(labelText + ":", value);
  values_[key] = value;
  return value;
}

void DeviceStatsPanel::updateFrame(const Frame &frame) {
  values_["frameNumber"]->setText(QString::number(frame.header.frameNumber));
  values_["detectedObjects"]->setText(
      QString::number(frame.header.numDetectedObj));

  if (!haveFrameClock_) {
    frameClock_.start();
    haveFrameClock_ = true;
  } else {
    const qint64 elapsedMs = frameClock_.restart();
    if (elapsedMs > 0) {
      const double fps = 1000.0 / double(elapsedMs);
      values_["frameTime"]->setText(
          tr("%1 ms (%2 FPS)").arg(elapsedMs).arg(fps, 0, 'f', 1));
    }
  }

  const auto &stats = frame.stats;
  values_["interFrameProcTime"]->setText(
      tr("%1 us").arg(stats.interFrameProcessingTime));
  values_["transmitOutputTime"]->setText(
      tr("%1 us").arg(stats.transmitOutputTime));
  values_["interFrameMargin"]->setText(
      tr("%1 us").arg(stats.interFrameProcessingMargin));
  values_["interChirpMargin"]->setText(
      tr("%1 us").arg(stats.interChirpProcessingMargin));
  values_["activeFrameCpuLoad"]->setText(
      tr("%1%").arg(stats.activeFrameCPULoad));
  values_["interFrameCpuLoad"]->setText(tr("%1%").arg(stats.interFrameCPULoad));

  const auto &temperature = frame.temperatureStats;
  values_["tempStatus"]->setText(
      temperature.tempReportValid == 0
          ? tr("Valid")
          : tr("Invalid (%1)").arg(temperature.tempReportValid));

  const RfTempData &report = temperature.temperatureReport;
  values_["tempRx0"]->setText(QString::number(report.tmpRx0Sens));
  values_["tempRx1"]->setText(QString::number(report.tmpRx1Sens));
  values_["tempRx2"]->setText(QString::number(report.tmpRx2Sens));
  values_["tempRx3"]->setText(QString::number(report.tmpRx3Sens));
  values_["tempTx0"]->setText(QString::number(report.tmpTx0Sens));
  values_["tempTx1"]->setText(QString::number(report.tmpTx1Sens));
  values_["tempTx2"]->setText(QString::number(report.tmpTx2Sens));
  values_["tempPm"]->setText(QString::number(report.tmpPmSens));
  values_["tempDig0"]->setText(QString::number(report.tmpDig0Sens));
  values_["tempDig1"]->setText(QString::number(report.tmpDig1Sens));
}
