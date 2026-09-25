#pragma once

#include <QElapsedTimer>
#include <QHash>
#include <QWidget>

#include "../core/TlvTypes.h"

class QLabel;
class QFormLayout;

class DeviceStatsPanel : public QWidget {
  Q_OBJECT
public:
  explicit DeviceStatsPanel(QWidget *parent = nullptr);

  void updateFrame(const Frame &frame);

private:
  QLabel *addStatRow_(QFormLayout *form, const QString &labelText,
                      const QString &key);

  QHash<QString, QLabel *> values_;
  // https://doc.qt.io/qt-6/qelapsedtimer.html
  QElapsedTimer frameClock_;
  bool haveFrameClock_ = false;
};
