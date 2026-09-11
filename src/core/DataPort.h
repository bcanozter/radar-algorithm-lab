#pragma once

#include <QObject>
#include <QtSerialPort/QSerialPort>

#include <vector>

class DataPort : public QObject {
  Q_OBJECT
public:
  explicit DataPort(QObject *parent = nullptr);

  bool connectPort(const QString &systemLocation, qint32 baudRate,
                   QString &errorMessage);
  void disconnectPort();
  bool isConnected() const { return port_.isOpen(); }

private slots:
  void onReadyRead();

private:
  QSerialPort port_;
  std::vector<uint8_t> rxBuffer_;
};
