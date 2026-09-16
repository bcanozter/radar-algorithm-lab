#include "ConfigPort.h"

#include "SerialPort.h"
#include <QDebug>

const char *const kCliPrompt = "mmwDemo:/>";

ConfigPort::ConfigPort(QObject *parent) : QObject(parent) {
  connect(&port_, &QSerialPort::readyRead, this, &ConfigPort::onReadyRead);
}

bool ConfigPort::connectPort(const QString &systemLocation, qint32 baudRate,
                             QString &errorMessage) {
  rxBuffer_.clear();
  return openSerialPort(port_, systemLocation, baudRate, errorMessage);
}

void ConfigPort::disconnectPort() {
  port_.close();
  rxBuffer_.clear();
}

void ConfigPort::onReadyRead() {
  const QByteArray data = port_.readAll();
  rxBuffer_.reserve(rxBuffer_.size() + data.size());
  rxBuffer_.insert(rxBuffer_.end(), data.cbegin(), data.cend());

  const QString text = QString::fromLatin1(rxBuffer_);
  qDebug() << "[Device][CFG]: " << text;
  logView_->appendMessage(text);
  const int idx = text.indexOf(QLatin1String(kCliPrompt));
  if (idx < 0) {
    // No prompt
    rxBuffer_.clear();
    return;
  }

  const QString responseText = text.left(idx).trimmed();
  rxBuffer_.clear();

  sendNextPendingLine_();
}

void ConfigPort::sendConfigFile(const QStringList &lines) {
  if (!isConnected()) {
    qWarning() << "Config port is not connected.";
    return;
  }
  qDebug() << "Sending .cfg contents to the device";

  pendingConfigLines_.clear();
  for (const QString &raw : lines) {
    const QString line = raw.trimmed();
    // Ignore comments
    if (line.isEmpty() || line.startsWith('%')) {
      continue;
    }
    pendingConfigLines_.push_back(line);
  }
  if (pendingConfigLines_.isEmpty()) {
    return;
  }

  sendNextPendingLine_();
}

void ConfigPort::sendNextPendingLine_() {
  if (pendingConfigLines_.isEmpty()) {
    return;
  }
  const QString line = pendingConfigLines_.takeFirst();
  qDebug() << "[Host][CFG]: " << line;
  port_.write(line.toLatin1() + '\n');
}


void ConfigPort::sendCommand(const QString& line) {
  if (!isConnected()) {
    qWarning() << "Config port is not connected.";
    return;
  }
  pendingConfigLines_ = {line.trimmed()};
  sendNextPendingLine_();
}
