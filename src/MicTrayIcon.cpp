#include "MicTrayIcon.h"
#include <QApplication>

MicTrayIcon::MicTrayIcon()
    : m_exitAction("Exit"), m_offIcon(QIcon::fromTheme("mic-indicator-off")),
      m_lowVolumeIcon(QIcon::fromTheme("mic-indicator-low")),
      m_highVolumeIcon(QIcon::fromTheme("mic-indicator-high")),
      m_micActive(false), m_volumeHigh(false) {

  setMicStatus(Off);
  m_trayIcon.show();

  QObject::connect(&m_exitAction, &QAction::triggered, qApp,
                   &QApplication::exit, Qt::DirectConnection);
  m_menu.addAction(&m_exitAction);
  m_trayIcon.setContextMenu(&m_menu);
}

void MicTrayIcon::setMicActive(bool micActive) {
  m_micActive = micActive;
  if (micActive) {
    setMicStatus(Low);
  } else {
    setMicStatus(Off);
    m_volumeHigh = false;
  }
}

void MicTrayIcon::setVolumeHigh(bool volumeHigh) {
  if (m_volumeHigh == volumeHigh || !m_micActive) {
    return;
  }

  m_volumeHigh = volumeHigh;
  if (volumeHigh) {
    setMicStatus(High);
  } else {
    setMicStatus(Low);
  }
}

void MicTrayIcon::setMicStatus(const MicStatus status) {
  switch (status) {
  case Off:
    m_trayIcon.setIcon(m_offIcon);
    m_trayIcon.setToolTip("Microphone is not in use");
    break;
  case Low:
    m_trayIcon.setIcon(m_lowVolumeIcon);
    m_trayIcon.setToolTip("Microphone is in use, no speech detected");
    break;
  case High:
    m_trayIcon.setIcon(m_highVolumeIcon);
    m_trayIcon.setToolTip("Microphone is in use, speech detected");
    break;
  }
}
