#include "DataPort.h"

#include "DataParser.h"
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
  const QByteArray data = port_.readAll();
  rxBuffer_.reserve(rxBuffer_.size() + data.size());
  rxBuffer_.insert(rxBuffer_.end(), data.cbegin(), data.cend());
  //
  for (const Frame &frame : parser_.parse(rxBuffer_)) {
    emit frameReceived(frame);
  }
}
