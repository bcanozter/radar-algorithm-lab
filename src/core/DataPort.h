#pragma once

#include <QObject>
#include <QtSerialPort/QSerialPort>

#include <vector>
#include "DataParser.h"
#include "TlvTypes.h"
class DataPort : public QObject {
  Q_OBJECT
public:
  explicit DataPort(QObject *parent = nullptr);

  bool connectPort(const QString &systemLocation, qint32 baudRate,
                   QString &errorMessage);
  void disconnectPort();
  bool isConnected() const { return port_.isOpen(); }

signals:
    void frameReceived(const Frame& frame);

private slots:
  void onReadyRead();

private:
  DataParser parser_;
  QSerialPort port_;
  std::vector<uint8_t> rxBuffer_;
};
