#pragma once

#include "PipeWireManager.h"

// NOLINTNEXTLINE
class AudioProcessor : public QObject {
  Q_OBJECT

public:
  AudioProcessor(PipeWireManager const *pipeWireManager, float volumeThreshold);

signals:
  void onVolumeChanged(bool highVolume);

public slots:
  void start(pw_stream *stream);

private slots:
  void processAudio();

  // NOLINTNEXTLINE
private:
  pw_stream *m_stream;
  float m_volumeThreshold;
  QTimer m_timer;
  uint m_lowCount;
};
