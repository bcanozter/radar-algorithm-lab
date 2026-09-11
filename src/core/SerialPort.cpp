#include "SerialPort.h"

#include <QDebug>
#include <QtSerialPort/QSerialPortInfo>

std::vector<AvailablePort> getAvailablePorts() {
  std::vector<AvailablePort> ports;
  const auto serialPortInfos = QSerialPortInfo::availablePorts();
  for (const QSerialPortInfo &portInfo : serialPortInfos) {
    qDebug() << "\n"
             << "Port:" << portInfo.portName() << "\n"
             << "Location:" << portInfo.systemLocation() << "\n"
             << "Description:" << portInfo.description() << "\n"
             << "Manufacturer:" << portInfo.manufacturer() << "\n"
             << "Serial number:" << portInfo.serialNumber() << "\n"
             << "Vendor Identifier:"
             << (portInfo.hasVendorIdentifier()
                     ? QByteArray::number(portInfo.vendorIdentifier(), 16)
                     : QByteArray())
             << "\n"
             << "Product Identifier:"
             << (portInfo.hasProductIdentifier()
                     ? QByteArray::number(portInfo.productIdentifier(), 16)
                     : QByteArray());
    ports.push_back({portInfo.portName(), portInfo.systemLocation(),
                     portInfo.description()});
  }
  return ports;
}

bool openSerialPort(QSerialPort &port, const QString &systemLocation,
                    qint32 baudRate, QString &errorMessage) {
  port.close();
  port.setPortName(systemLocation);
  port.setBaudRate(baudRate);
  port.setDataBits(QSerialPort::Data8);
  port.setParity(QSerialPort::NoParity);
  port.setStopBits(QSerialPort::OneStop);
  port.setFlowControl(QSerialPort::NoFlowControl);

  if (!port.open(QIODevice::ReadWrite)) {
    errorMessage = port.errorString();
    return false;
  }
  return true;
}
