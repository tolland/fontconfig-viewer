# FontConfig Viewer

A simple GTK4 application that displays all available fonts on your Linux system using the fontconfig library.

## Prerequisites

Make sure you have the following development packages installed:

```bash
# On Ubuntu/Debian:
sudo apt-get install build-essential cmake pkg-config libgtk-4-dev libfontconfig1-dev

# On Fedora:
sudo dnf install gcc-c++ cmake pkg-config gtk4-devel fontconfig-devel

# On Arch Linux:
sudo pacman -S base-devel cmake pkg-config gtk4 fontconfig
```

## Building

```bash
mkdir build
cd build
cmake ..
make
```

## Running

```bash
./fontconfig_viewer
```

## Features

- Lists all fonts available on your system
- Shows font family, style, and file path
- Scrollable interface for large font collections
- Clean GTK4 interface

## How it works

The application uses:
- **GTK4** for the graphical user interface
- **fontconfig** library to query system fonts
- **CMake** for build system management

The fontconfig library provides a unified interface for font discovery and configuration on Linux systems.
