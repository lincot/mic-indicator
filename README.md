# PipeWire microphone tray indicator

A simple microphone indicator for Linux using PipeWire, displayed
in the system tray. It shows if any applications are using the microphone
and if you can be heard.

![showcase](showcase.gif)

## Usage

```
Usage: mic-indicator [options]
PipeWire microphone tray indicator

Options:
  -h, --help                           Displays help on commandline options.
  --help-all                           Displays help, including generic Qt
                                       options.
  -e, --exclude-nodes <nodes>          Comma-separated list of input node names
                                       to ignore (default:
                                       mic-indicator,capture.rnnoise_source)
  -v, --volume-threshold <threshold>   The volume threshold for speech
                                       detection (default: 0.001)
  -s, --sampling-rate <sampling rate>  Sampling rate (default: 48000)
  -c, --channels <channels>            Number of channels (default: 1)
```

## Installation

### Manual installation

Run these commands:

```sh
git clone https://github.com/lincot/mic-indicator
cd mic-indicator
cmake -B build -S . -DCMAKE_BUILD_TYPE=Release -DCMAKE_CXX_FLAGS="-march=native"
cmake --build build
sudo cp build/mic-indicator /usr/bin/
sudo cp res/icons/* /usr/share/icons/hicolor/64x64/apps/
```
