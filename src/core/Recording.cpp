#include "Recording.h"

#include "DataParser.h"

#include <QDebug>
#include <QFileInfo>

static QString cfgPathForRecording(const QString &recordingPath) {
  const QFileInfo info(recordingPath);
  QString filePath = info.path() + "/" + info.completeBaseName() + ".cfg";
  return filePath;
}

Recording::Recording(QObject *parent) : QObject(parent) {
  timer_.setSingleShot(true);
  connect(&timer_, &QTimer::timeout, this, &Recording::onTimeout);
}

bool Recording::startRecording(const QString &path, const QStringList &cfgLines,
                               QString &errorMessage) {
  stopRecording();

  file_.setFileName(path);
  if (!file_.open(QIODevice::WriteOnly)) {
    errorMessage = file_.errorString();
    return false;
  }
  stream_.setDevice(&file_);

  if (cfgLines.isEmpty()) {
    qWarning() << "No cfg has been sent yet, recording without a .cfg file";
  } else {
    QFile cfgFile(cfgPathForRecording(path));
    if (cfgFile.open(QIODevice::WriteOnly | QIODevice::Text)) {
      cfgFile.write((cfgLines.join('\n') + '\n').toUtf8());
    } else {
      qWarning() << "Unable to write cfg file:" << cfgFile.errorString();
    }
  }

  clock_.start();
  return true;
}

void Recording::writeFrame(const Frame &frame) {
  if (!isRecording()) {
    return;
  }
  const qint64 timestampUs = clock_.nsecsElapsed() / 1000;
  stream_ << timestampUs << static_cast<quint32>(frame.rawBytes.size());
  for (const uint8_t byte : frame.rawBytes) {
    stream_ << byte;
  }
}

void Recording::stopRecording() { file_.close(); }

bool Recording::load(const QString &path, QString &errorMessage) {
  unload();

  QFile file(path);
  if (!file.open(QIODevice::ReadOnly)) {
    errorMessage = file.errorString();
    return false;
  }
  QDataStream in(&file);

  DataParser parser;
  while (!in.atEnd()) {
    qint64 timestampUs = 0;
    quint32 size = 0;
    in >> timestampUs >> size;

    if (in.status() != QDataStream::Ok || size > file.size() - file.pos()) {
      break;
    }
    std::vector<uint8_t> packet(size);
    for (uint8_t &byte : packet) {
      in >> byte;
    }
    for (Frame &frame : parser.parse(packet)) {
      frames_.push_back({timestampUs, std::move(frame)});
    }
  }

  if (frames_.empty()) {
    errorMessage = tr("Recording contains no frames");
    return false;
  }

  QFile cfgFile(cfgPathForRecording(path));
  if (cfgFile.open(QIODevice::ReadOnly | QIODevice::Text)) {
    cfgLines_ = QString::fromUtf8(cfgFile.readAll()).split('\n');
  }

  emit loaded();
  return true;
}

void Recording::unload() {
  pause();
  frames_.clear();
  cfgLines_.clear();
  index_ = 0;
  emit unloaded();
}

void Recording::play() {
  if (frames_.empty() || playing_) {
    return;
  }
  // start over
  if (index_ == frameCount() - 1) {
    seek(0);
  }
  playing_ = true;
  emit playingChanged(true);
  scheduleNext_();
}

void Recording::pause() {
  timer_.stop();
  if (playing_) {
    playing_ = false;
    emit playingChanged(false);
  }
}

void Recording::seek(int index) {
  if (index < 0 || index >= frameCount()) {
    return;
  }
  index_ = index;
  emit frameReceived(frames_[index_].frame);
  emit positionChanged(index_);
  scheduleNext_();
}

void Recording::onTimeout() { seek((index_ + 1) % frameCount()); }

void Recording::scheduleNext_() {
  if (!playing_) {
    return;
  }
  const bool atLastFrame = index_ == frameCount() - 1;
  if (atLastFrame && !loop_) {
    pause();
    return;
  }

  qint64 gapUs = 0;
  if (!atLastFrame) {
    gapUs = frames_[index_ + 1].timestampUs - frames_[index_].timestampUs;
  }
  timer_.start(static_cast<int>(gapUs / 1000.0 / speed_));
}
