#pragma once

#include <QWidget>

class QPlainTextEdit;

class LogView : public QWidget {
  Q_OBJECT
public:
  explicit LogView(QWidget *parent = nullptr);

public slots:
  void appendMessage(const QString &text);

private:
  QPlainTextEdit *text_;
};
