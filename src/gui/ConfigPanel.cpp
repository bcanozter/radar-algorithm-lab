#include "ConfigPanel.h"

#include <QPlainTextEdit>
#include <QVBoxLayout>

ConfigPanel::ConfigPanel(QWidget *parent) : QWidget(parent) {
  editor_ = new QPlainTextEdit(this);
  editor_->setPlaceholderText(tr("Load a .cfg file, or paste one here..."));
  editor_->setLineWrapMode(QPlainTextEdit::NoWrap);
  QFont mono(QStringLiteral("monospace"));
  mono.setStyleHint(QFont::TypeWriter);
  editor_->setFont(mono);

  auto *layout = new QVBoxLayout(this);
  layout->addWidget(editor_, 1);
}
