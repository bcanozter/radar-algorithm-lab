#include "LogView.h"

#include <QDateTime>
#include <QPlainTextEdit>
#include <QVBoxLayout>

LogView::LogView(QWidget *parent) : QWidget(parent) {
  text_ = new QPlainTextEdit(this);
  text_->setReadOnly(true);
  text_->setMaximumBlockCount(5000);
  QFont mono(QStringLiteral("monospace"));
  mono.setStyleHint(QFont::TypeWriter);
  text_->setFont(mono);

  auto *layout = new QVBoxLayout(this);
  layout->addWidget(text_);
}
