#pragma once

#include <QWidget>

class QComboBox;
class QLabel;
class QPushButton;
class QSlider;
class Recording;

class ReplayBar : public QWidget {
  Q_OBJECT
public:
  explicit ReplayBar(Recording *recording, QWidget *parent = nullptr);

private:
  void onLoaded_();
  void onUnloaded_();
  void onPositionChanged_(int index);

  Recording *recording_;

  QPushButton *playButton_;
  QSlider *positionSlider_;
  QLabel *positionLabel_;
  QComboBox *speedCombo_;
};
