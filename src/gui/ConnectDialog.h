#pragma once

#include <QDialog>

class QComboBox;

class ConnectDialog : public QDialog {
  Q_OBJECT
public:
  explicit ConnectDialog(QWidget *parent = nullptr);

  QString getConfigPortAddress() const;
  QString getDataPortAddress() const;
  int getConfigPortBaudRate() const;
  int getDataPortBaudRate() const;

private slots:
  void handleRefresh_();

private:
  QComboBox *configPortCombo_;
  QComboBox *configPortBaudCombo_;
  QComboBox *dataPortCombo_;
  QComboBox *dataPortBaudCombo_;
};
