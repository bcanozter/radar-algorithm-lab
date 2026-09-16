#pragma once

#include <QObject>
#include <QtSerialPort/QSerialPort>

#include <vector>
#include "../gui/LogView.h"

class LogView;

class ConfigPort : public QObject {
  Q_OBJECT
public:
  explicit ConfigPort(QObject *parent = nullptr);

  bool connectPort(const QString &systemLocation, qint32 baudRate,
                   QString &errorMessage);
  void disconnectPort();
  bool isConnected() const { return port_.isOpen(); }
  void sendConfigFile(const QStringList& lines);
  void sendCommand(const QString& line);
  void setLogView(LogView *logView) { logView_ = logView; }

private slots:
  void onReadyRead();

private:

  LogView* logView_;

  void sendNextPendingLine_();

  QSerialPort port_;
  std::vector<uint8_t> rxBuffer_;
  QStringList pendingConfigLines_;
};
