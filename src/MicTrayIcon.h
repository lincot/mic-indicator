#pragma once

#include <QIcon>
#include <QMenu>
#include <QSystemTrayIcon>

enum MicStatus {
  Off,
  Low,
  High,
};

class MicTrayIcon : public QObject {
  Q_OBJECT

public:
  MicTrayIcon();

  // NOLINTNEXTLINE
public slots:
  void setMicActive(bool micActive);
  void setVolumeHigh(bool volumeHigh);

private:
  void setMicStatus(MicStatus micStatus);

  QSystemTrayIcon m_trayIcon;
  QMenu m_menu;
  QAction m_exitAction;
  QIcon m_offIcon;
  QIcon m_lowVolumeIcon;
  QIcon m_highVolumeIcon;

  bool m_micActive;
  bool m_volumeHigh;
};
