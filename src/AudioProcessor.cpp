#include "AudioProcessor.h"
#include "PipeWireManager.h"

AudioProcessor::AudioProcessor(PipeWireManager const *pipeWireManager,
                               float volumeThreshold)
    : m_volumeThreshold(volumeThreshold), m_lowCount(0) {

  connect(&m_timer, &QTimer::timeout, this, &AudioProcessor::processAudio,
          Qt::DirectConnection);
  connect(
      pipeWireManager, &PipeWireManager::onMicUsageChanged, &m_timer,
      [this](bool micActive) {
        if (micActive) {
          m_timer.start(100);
        } else {
          m_timer.stop();
        }
      },
      Qt::QueuedConnection);
}

void AudioProcessor::start(pw_stream *stream) { m_stream = stream; }

float calculateRMS(const float *const audioSamples,
                   const uint32_t sampleCount) {
  float sum = 0;
  for (uint32_t i = 0; i < sampleCount; ++i) {
    sum += audioSamples[i] * audioSamples[i];
  }
  return sqrtf(sum / static_cast<float>(sampleCount));
}

void AudioProcessor::processAudio() {
  pw_buffer *buffer;
  spa_buffer *spaBuffer;

  buffer = pw_stream_dequeue_buffer(m_stream);
  if (buffer == nullptr) {
    return;
  }

  spaBuffer = buffer->buffer;
  if (spaBuffer->datas[0].data == nullptr) {
    pw_stream_queue_buffer(m_stream, buffer);
    return;
  }

  const auto *audioSamples = static_cast<float *>(spaBuffer->datas[0].data);
  const uint32_t sampleCount = spaBuffer->datas[0].chunk->size / sizeof(float);

  const auto volume = calculateRMS(audioSamples, sampleCount);

  pw_stream_queue_buffer(m_stream, buffer);

  if (volume < m_volumeThreshold) {
    if (++m_lowCount == 5) {
      emit onVolumeChanged(false);
    }
  } else {
    m_lowCount = 0;
    emit onVolumeChanged(true);
  }
}
