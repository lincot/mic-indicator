#pragma once

#include <QApplication>

struct Config {
  QStringList excludedNodes;
  float volumeThreshold;
  uint32_t samplingRate;
  uint32_t channels;
};

Config getConfig(const QApplication &app);
