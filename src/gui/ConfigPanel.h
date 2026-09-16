#pragma once

#include <QStringList>
#include <QWidget>

class QPlainTextEdit;
class QLabel;
class QPushButton;

class ConfigPanel : public QWidget {
  Q_OBJECT
public:
  explicit ConfigPanel(QWidget *parent = nullptr);
  QStringList getEditorLines() const;

public slots:

signals:
  void sendConfigRequested(const QStringList& lines);
  void startSensorRequested();
  void stopSensorRequested();
private slots:
  void handleLoadConfig_();
  void handleSendConfig_();
  void handleStartSensor_();
  void handleStopSensor_();
  void loadFileToEditor(const QString &path);

private:
  QPlainTextEdit *editor_;
  QPushButton *loadConfigButton_;
  QPushButton *sendConfigButton_;
  QPushButton *startSensorButton_;
  QPushButton *stopSensorButton_;
  QString lastDir_;
};
