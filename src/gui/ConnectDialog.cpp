#include "ConnectDialog.h"

#include <QComboBox>
#include <QDialogButtonBox>
#include <QFormLayout>
#include <QPushButton>
#include <QVBoxLayout>

#include "../core/SerialPort.h"

const int kCommonBaudRates[] = {9600,    19200,   38400,   57600,
                                115200,  230400,  460800,  921600,
                                1000000, 1500000, 2000000, 3000000};

QComboBox *makeBaudCombo(QWidget *parent, int defaultBaud) {
  auto *combo = new QComboBox(parent);
  for (int baudRate : kCommonBaudRates) {
    combo->addItem(QString::number(baudRate), baudRate);
  }
  const int idx = combo->findData(defaultBaud);
  combo->setCurrentIndex(idx >= 0 ? idx : 0);
  return combo;
}

ConnectDialog::ConnectDialog(QWidget *parent) : QDialog(parent) {
  setWindowTitle(tr("Connect to mmWave Device"));
  resize(480, 240);

  configPortCombo_ = new QComboBox(this);
  dataPortCombo_ = new QComboBox(this);

  configPortBaudCombo_ = makeBaudCombo(this, kConfigPortBaudRate);
  dataPortBaudCombo_ = makeBaudCombo(this, kDataPortBaudRate);

  auto *refreshButton = new QPushButton(tr("Refresh"), this);
  connect(refreshButton, &QPushButton::clicked, this,
          &ConnectDialog::handleRefresh_);

  auto *form = new QFormLayout;
  form->addRow(tr("Config Port:"), configPortCombo_);
  form->addRow(tr("Baud Rate:"), configPortBaudCombo_);
  form->addRow(tr("Data port:"), dataPortCombo_);
  form->addRow(tr("Baud Rate:"), dataPortBaudCombo_);

  auto *buttons = new QDialogButtonBox(
      QDialogButtonBox::Ok | QDialogButtonBox::Cancel, this);
  connect(buttons, &QDialogButtonBox::accepted, this, &QDialog::accept);
  connect(buttons, &QDialogButtonBox::rejected, this, &QDialog::reject);

  auto *layout = new QVBoxLayout(this);
  layout->addLayout(form);
  layout->addWidget(refreshButton);
  layout->addWidget(buttons);

  // Populate the list
  handleRefresh_();
}

void ConnectDialog::handleRefresh_() {
  const QString prevConfigPort = configPortCombo_->currentData().toString();
  const QString prevDataPort = dataPortCombo_->currentData().toString();

  configPortCombo_->clear();
  dataPortCombo_->clear();

  for (const AvailablePort &p : getAvailablePorts()) {
    const QString label =
        QStringLiteral("%1 (%2)").arg(p.systemLocation, p.description);
    configPortCombo_->addItem(label, p.systemLocation);
    dataPortCombo_->addItem(label, p.systemLocation);
  }

  if (dataPortCombo_->count() > 1) {
    dataPortCombo_->setCurrentIndex(1);
  }

  if (!prevConfigPort.isEmpty()) {
    const int idx = configPortCombo_->findData(prevConfigPort);
    if (idx >= 0)
      configPortCombo_->setCurrentIndex(idx);
  }
  if (!prevDataPort.isEmpty()) {
    const int idx = dataPortCombo_->findData(prevDataPort);
    if (idx >= 0)
      dataPortCombo_->setCurrentIndex(idx);
  }
}

QString ConnectDialog::getConfigPortAddress() const {
  return configPortCombo_->currentData().toString();
}

QString ConnectDialog::getDataPortAddress() const {
  return dataPortCombo_->currentData().toString();
}

int ConnectDialog::getConfigPortBaudRate() const {
  return configPortBaudCombo_->currentData().toInt();
}

int ConnectDialog::getDataPortBaudRate() const {
  return dataPortBaudCombo_->currentData().toInt();
}
