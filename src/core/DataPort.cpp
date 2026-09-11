#include "DataPort.h"

#include "SerialPort.h"

DataPort::DataPort(QObject *parent) : QObject(parent) {
  connect(&port_, &QSerialPort::readyRead, this, &DataPort::onReadyRead);
}

bool DataPort::connectPort(const QString &systemLocation, qint32 baudRate,
                           QString &errorMessage) {
  rxBuffer_.clear();
  return openSerialPort(port_, systemLocation, baudRate, errorMessage);
}

void DataPort::disconnectPort() {
  port_.close();
  rxBuffer_.clear();
}

void DataPort::onReadyRead() {
  // todo
}
