#include "AudioProcessor.h"
#include "Config.h"
#include "MicTrayIcon.h"
#include "PipeWireManager.h"
#include <QThread>
#include <csignal>

int main(int argc, char *argv[]) {
  const QApplication app(argc, argv);

  for (auto sig : {SIGINT, SIGTERM}) {
    (void)std::signal(sig, [](int) { QApplication::exit(); });
  }

  const Config config = getConfig(app);

  const MicTrayIcon micTrayIcon;

  PipeWireManager pipeWireManager(config.excludedNodes, config.samplingRate,
                                  config.channels);
  QObject::connect(&pipeWireManager, &PipeWireManager::onMicUsageChanged,
                   &micTrayIcon, &MicTrayIcon::setMicActive,
                   Qt::QueuedConnection);

  QThread pipeWireThread;
  pipeWireManager.moveToThread(&pipeWireThread);
  QObject::connect(&pipeWireThread, &QThread::started, &pipeWireManager,
                   &PipeWireManager::run, Qt::DirectConnection);

  const AudioProcessor audioProcessor(&pipeWireManager, config.volumeThreshold);
  QObject::connect(&pipeWireManager, &PipeWireManager::onStreamCreated,
                   &audioProcessor, &AudioProcessor::start,
                   Qt::QueuedConnection);
  QObject::connect(&audioProcessor, &AudioProcessor::onVolumeChanged,
                   &micTrayIcon, &MicTrayIcon::setVolumeHigh,
                   Qt::DirectConnection);

  pipeWireThread.start();

  const int retCode = QApplication::exec();

  pipeWireManager.stop();
  pipeWireThread.quit();
  pipeWireThread.wait();

  return retCode;
}
