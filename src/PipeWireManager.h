#pragma once

#include <QTimer>
#include <pipewire/pipewire.h>

class PipeWireManager : public QObject {
  Q_OBJECT
public:
  PipeWireManager(QStringList excludedNodes, uint32_t samplingRate,
                  uint32_t channels);
  void stop();
  void run();

signals:
  void onMicUsageChanged(bool micUsed);
  void onStreamCreated(pw_stream *stream);

private:
  void setupAudioStream();

  void onGlobalEvent(uint32_t id, const spa_dict *props);
  void onGlobalRemoveEvent(uint32_t id);

  pw_main_loop *m_loop;
  pw_context *m_context;
  pw_core *m_core;
  pw_registry *m_registry;
  spa_hook m_registry_listener;
  pw_stream *m_stream;

  QStringList m_excludedNodes;
  uint32_t m_samplingRate;
  uint32_t m_channels;

  // There are usually not many nodes, so a vector is sufficient.
  std::vector<uint32_t> m_nodes;
};
