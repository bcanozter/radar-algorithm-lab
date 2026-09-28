#pragma once

#include <QDataStream>
#include <QElapsedTimer>
#include <QFile>
#include <QObject>
#include <QStringList>
#include <QTimer>

#include <vector>

#include "TlvTypes.h"

class Recording : public QObject {
  Q_OBJECT
public:
  explicit Recording(QObject *parent = nullptr);

  // record
  bool startRecording(const QString &path, const QStringList &cfgLines,
                      QString &errorMessage);
  void writeFrame(const Frame &frame);
  void stopRecording();
  bool isRecording() const { return file_.isOpen(); }
  QString path() const { return file_.fileName(); }
  qint64 elapsedMs() const { return clock_.elapsed(); }
  qint64 bytesWritten() const { return file_.pos(); }

  // replay
  bool load(const QString &path, QString &errorMessage);
  void unload();
  void play();
  void pause();
  void seek(int index);
  void setSpeed(double speed) { speed_ = speed; }
  void setLoop(bool loop) { loop_ = loop; }
  const QStringList &cfgLines() const { return cfgLines_; }
  int frameCount() const { return static_cast<int>(frames_.size()); }
  qint64 timestampUs(int index) const { return frames_[index].timestampUs; }
  bool isPlaying() const { return playing_; }

signals:
  void frameReceived(const Frame &frame);
  void loaded();
  void unloaded();
  void positionChanged(int index);
  void playingChanged(bool playing);

private slots:
  void onTimeout();

private:
  struct RecordedFrame {
    qint64 timestampUs;
    Frame frame;
  };

  void scheduleNext_();

  // record
  QFile file_;
  QDataStream stream_;
  QElapsedTimer clock_;

  // replay
  std::vector<RecordedFrame> frames_;
  QStringList cfgLines_;
  QTimer timer_;
  int index_ = 0;
  double speed_ = 1.0;
  bool loop_ = false;
  bool playing_ = false;
};
