#include "PipeWireManager.h"
#include <QDebug>
#include <spa/param/audio/format-utils.h>

PipeWireManager::PipeWireManager(QStringList excludedNodes,
                                 uint32_t samplingRate, uint32_t channels)
    : m_excludedNodes(std::move(excludedNodes)), m_samplingRate(samplingRate),
      m_channels(channels) {}

void PipeWireManager::run() {
  pw_init(nullptr, nullptr);

  m_loop = pw_main_loop_new(nullptr);
  m_context = pw_context_new(pw_main_loop_get_loop(m_loop), nullptr, 0);
  if (m_context == nullptr) {
    qCritical() << "Failed to initialize PipeWire context.";
    return;
  }
  m_core = pw_context_connect(m_context, nullptr, 0);
  if (m_core == nullptr) {
    qCritical() << "Failed to connect to PipeWire.";
    return;
  }
  m_registry = pw_core_get_registry(m_core, PW_VERSION_REGISTRY, 0);

  auto events = pw_registry_events{
      .global =
          [](void *data, uint32_t id, uint32_t /*permissions*/,
             const char * /*type*/, uint32_t /*version*/,
             const spa_dict *props) {
            static_cast<PipeWireManager *>(data)->onGlobalEvent(id, props);
          },
      .global_remove =
          [](void *data, uint32_t id) {
            static_cast<PipeWireManager *>(data)->onGlobalRemoveEvent(id);
          }};
  spa_zero(m_registry_listener);

  // NOLINTNEXTLINE
  pw_registry_add_listener(m_registry, &m_registry_listener, &events, this);
  setupAudioStream();

  pw_main_loop_run(m_loop);

  pw_stream_destroy(m_stream);
  pw_proxy_destroy(reinterpret_cast<pw_proxy *>(m_registry));
  pw_core_disconnect(m_core);
  pw_context_destroy(m_context);
  pw_main_loop_destroy(m_loop);
  pw_deinit();
}

void PipeWireManager::stop() { pw_main_loop_quit(m_loop); }

void PipeWireManager::setupAudioStream() {
  pw_init(nullptr, nullptr);

  m_stream = pw_stream_new(m_core, "Mic Indicator", nullptr);

  std::array<spa_pod *, 1> params;
  std::array<uint8_t, 1024> buffer;

  spa_pod_builder b = SPA_POD_BUILDER_INIT(buffer.data(), sizeof(buffer));

  auto info =
      SPA_AUDIO_INFO_RAW_INIT(.format = SPA_AUDIO_FORMAT_F32,
                              .rate = m_samplingRate, .channels = m_channels);
  params[0] = spa_format_audio_raw_build(&b, SPA_PARAM_EnumFormat, &info);

  pw_stream_connect(m_stream, PW_DIRECTION_INPUT, PW_ID_ANY,
                    // NOLITNEXTLINE
                    static_cast<pw_stream_flags>(PW_STREAM_FLAG_AUTOCONNECT |
                                                 PW_STREAM_FLAG_MAP_BUFFERS),
                    const_cast<const spa_pod **>(params.data()), params.size());

  emit onStreamCreated(m_stream);
}

void PipeWireManager::onGlobalEvent(const uint32_t id,
                                    const spa_dict *const props) {
  const char *mediaClass = spa_dict_lookup(props, PW_KEY_MEDIA_CLASS);

  if (mediaClass == nullptr || strcmp(mediaClass, "Stream/Input/Audio") != 0) {
    return;
  }

  const char *nodeName = spa_dict_lookup(props, PW_KEY_NODE_NAME);
  if (m_excludedNodes.contains(nodeName)) {
    return;
  }

  if (m_nodes.empty()) {
    emit onMicUsageChanged(true);
  }

  if (std::find(m_nodes.begin(), m_nodes.end(), id) == m_nodes.end()) {
    m_nodes.push_back(id);
  }
}

void PipeWireManager::onGlobalRemoveEvent(const uint32_t id) {
  auto it = std::find(m_nodes.begin(), m_nodes.end(), id);
  if (it != m_nodes.end()) {
    *it = m_nodes.back();
    m_nodes.pop_back();
  }

  if (m_nodes.empty()) {
    emit onMicUsageChanged(false);
  }
}
