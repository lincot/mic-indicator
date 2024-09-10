#include "Config.h"
#include <QCommandLineParser>

Config getConfig(const QApplication &app) {
  QCommandLineParser parser;
  parser.setApplicationDescription("PipeWire microphone tray indicator");
  parser.addHelpOption();

  const QCommandLineOption excludedNodesOption(
      QStringList() << "e" << "exclude-nodes",
      "Comma-separated list of input node names to ignore (default: "
      "mic-indicator,capture.rnnoise_source)",
      "nodes", "mic-indicator,capture.rnnoise_source");
  parser.addOption(excludedNodesOption);

  const QCommandLineOption volumeThresholdOption(
      QStringList() << "v" << "volume-threshold",
      "The volume threshold for speech detection (default: 0.001)", "threshold",
      "0.001");
  parser.addOption(volumeThresholdOption);

  const QCommandLineOption samplingRateOption(
      QStringList() << "s" << "sampling-rate", "Sampling rate (default: 48000)",
      "sampling rate", "48000");
  parser.addOption(samplingRateOption);

  const QCommandLineOption channelsOption(QStringList() << "c" << "channels",
                                          "Number of channels (default: 1)",
                                          "channels", "1");
  parser.addOption(channelsOption);

  parser.process(app);

  return Config{
      .excludedNodes = parser.value(excludedNodesOption).split(','),
      .volumeThreshold = parser.value(volumeThresholdOption).toFloat(),
      .samplingRate = parser.value(samplingRateOption).toUInt(),
      .channels = parser.value(channelsOption).toUInt(),
  };
}
