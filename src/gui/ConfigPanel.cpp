#include "ConfigPanel.h"

#include <QFile>
#include <QFileDialog>
#include <QFileInfo>
#include <QPlainTextEdit>
#include <QPushButton>
#include <QVBoxLayout>

ConfigPanel::ConfigPanel(QWidget *parent) : QWidget(parent) {
  editor_ = new QPlainTextEdit(this);
  editor_->setPlaceholderText(tr("Load a .cfg file, or paste one here..."));
  editor_->setLineWrapMode(QPlainTextEdit::NoWrap);
  QFont mono(QStringLiteral("monospace"));
  mono.setStyleHint(QFont::TypeWriter);
  editor_->setFont(mono);

  loadConfigButton_ = new QPushButton(tr("Load Config"), this);
  sendConfigButton_ = new QPushButton(tr("Send to Device"), this);
  startSensorButton_ = new QPushButton(tr("Start Sensor"), this);
  stopSensorButton_ = new QPushButton(tr("Stop Sensor"), this);

  connect(loadConfigButton_, &QPushButton::clicked, this,
          &ConfigPanel::handleLoadConfig_);
  connect(sendConfigButton_, &QPushButton::clicked, this,
          &ConfigPanel::handleSendConfig_);
  connect(startSensorButton_, &QPushButton::clicked, this,
          &ConfigPanel::handleStartSensor_);
  connect(stopSensorButton_, &QPushButton::clicked, this,
          &ConfigPanel::handleStopSensor_);

  auto *buttonRow = new QHBoxLayout;
  buttonRow->addWidget(loadConfigButton_);
  buttonRow->addWidget(sendConfigButton_);
  buttonRow->addStretch();
  buttonRow->addWidget(startSensorButton_);
  buttonRow->addWidget(stopSensorButton_);

  auto *layout = new QVBoxLayout(this);
  layout->addWidget(editor_, 1);
  layout->addLayout(buttonRow);
}

void ConfigPanel::loadFileToEditor(const QString &path) {
  QFile file(path);
  if (!file.open(QIODevice::ReadOnly | QIODevice::Text)) {
    return;
  }
  editor_->setPlainText(QTextStream(&file).readAll());
  lastDir_ = QFileInfo(path).absolutePath();
}

void ConfigPanel::handleLoadConfig_() {
  const QString path =
      QFileDialog::getOpenFileName(this, tr("Load config file"), lastDir_,
                                   tr(".cfg (*.cfg);;All files (*)"));
  if (!path.isEmpty()) {
    loadFileToEditor(path);
  }
}

void ConfigPanel::handleSendConfig_() {
  // TODO
}

void ConfigPanel::handleStartSensor_() {
  // TODO
}

void ConfigPanel::handleStopSensor_() {
  // TODO
}
