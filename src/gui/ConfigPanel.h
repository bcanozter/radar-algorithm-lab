#pragma once

#include <QStringList>
#include <QWidget>

class QPlainTextEdit;
class QLabel;

class ConfigPanel : public QWidget {
  Q_OBJECT
public:
  explicit ConfigPanel(QWidget *parent = nullptr);

public slots:

signals:

private slots:

private:
  QPlainTextEdit *editor_;
};
