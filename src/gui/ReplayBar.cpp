#include "ReplayBar.h"

#include <QCheckBox>
#include <QComboBox>
#include <QHBoxLayout>
#include <QLabel>
#include <QPushButton>
#include <QSignalBlocker>
#include <QSlider>

#include "../core/Recording.h"

ReplayBar::ReplayBar(Recording *recording, QWidget *parent)
    : QWidget(parent), recording_(recording) {
  playButton_ = new QPushButton(tr("Play"), this);

  positionSlider_ = new QSlider(Qt::Horizontal, this);
  positionSlider_->setMinimumWidth(300);

  positionLabel_ = new QLabel(tr("No recording"), this);

  speedCombo_ = new QComboBox(this);
  for (const double speed : {0.25, 0.5, 1.0, 2.0, 4.0}) {
    speedCombo_->addItem(QString("%1x").arg(speed), speed);
  }
  speedCombo_->setCurrentIndex(2);

  auto *loopCheck = new QCheckBox(tr("Loop"), this);

  auto *layout = new QHBoxLayout(this);
  layout->setContentsMargins(0, 0, 0, 0);
  layout->addWidget(playButton_);
  layout->addWidget(positionSlider_, 1);
  layout->addWidget(positionLabel_);
  layout->addWidget(speedCombo_);
  layout->addWidget(loopCheck);

  connect(playButton_, &QPushButton::clicked, this, [this] {
    if (recording_->isPlaying()) {
      recording_->pause();
    } else {
      recording_->play();
    }
  });

  connect(positionSlider_, &QSlider::valueChanged, recording_,
          &Recording::seek);
  connect(speedCombo_, &QComboBox::currentIndexChanged, this, [this] {
    recording_->setSpeed(speedCombo_->currentData().toDouble());
  });
  connect(loopCheck, &QCheckBox::toggled, recording_, &Recording::setLoop);

  connect(recording_, &Recording::loaded, this, &ReplayBar::onLoaded_);
  connect(recording_, &Recording::unloaded, this, &ReplayBar::onUnloaded_);
  connect(recording_, &Recording::positionChanged, this,
          &ReplayBar::onPositionChanged_);
  connect(recording_, &Recording::playingChanged, this, [this](bool playing) {
    playButton_->setText(playing ? tr("Pause") : tr("Play"));
  });

  setEnabled(false);
}

void ReplayBar::onLoaded_() {
  const QSignalBlocker blocker(positionSlider_);
  positionSlider_->setRange(0, recording_->frameCount() - 1);
  setEnabled(true);
}

void ReplayBar::onUnloaded_() {
  const QSignalBlocker blocker(positionSlider_);
  positionSlider_->setRange(0, 0);
  positionLabel_->setText(tr("No recording"));
  setEnabled(false);
}

void ReplayBar::onPositionChanged_(int index) {
  const QSignalBlocker blocker(positionSlider_);
  positionSlider_->setValue(index);

  const double seconds =
      (recording_->timestampUs(index) - recording_->timestampUs(0)) / 1e6;
  positionLabel_->setText(QString("%1 / %2   %3 s")
                              .arg(index + 1)
                              .arg(recording_->frameCount())
                              .arg(seconds, 0, 'f', 2));
}
