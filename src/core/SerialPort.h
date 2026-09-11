#pragma once

#include <QString>
#include <QtSerialPort/QSerialPort>
#include <vector>

// mss_main.c: config->commandBaudRate = 115200, config->loggingBaudRate =
// 921600.
static constexpr int kConfigPortBaudRate = 115200;
static constexpr int kDataPortBaudRate = 921600;

struct AvailablePort {
  QString portName;
  QString systemLocation;
  QString description;
};

std::vector<AvailablePort> getAvailablePorts();

bool openSerialPort(QSerialPort &port, const QString &systemLocation,
                    qint32 baudRate, QString &errorMessage);
