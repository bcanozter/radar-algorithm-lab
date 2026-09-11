#include "ConfigPort.h"

#include "SerialPort.h"

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
  // todo
}
