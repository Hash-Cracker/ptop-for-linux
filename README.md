# System Monitor

A simple terminal-based system monitor program written in C that displays live CPU usage, memory usage (in GB), system uptime, and running processes with their PID and names. The program is designed to run in a terminal and update the information in real-time using the `ncurses` library for better user interaction.

## Features

- **CPU Usage**: Displays the percentage of CPU usage in real-time.
- **Memory Usage**: Displays the amount of memory used and free in GB.
- **System Uptime**: Shows the system uptime in seconds.
- **Running Processes**: Lists the running processes along with their PID and names.
- **Real-time Updates**: Information updates every few seconds.
- **Ncurses-based Interface**: A terminal interface that dynamically updates the system stats.

## Requirements

- GCC compiler (`gcc`)
- `ncurses` library for terminal-based UI
- For Ubuntu/Debian-based systems: 
```bash 
sudo apt-get install libncurses5-dev
```
- For Arch-based: 
```bash
sudo pacman -S ncurses
```
- For Fedora: 
```bash
sudo dnf install ncurses-devel
``````
- For RHEL/CentOS:
```bash
sudo yum install ncurses-devel
```
- For openSUSE:
```bash
sudo zypper install ncurses-devel
```
- For Alpine Linux:
```bash
sudo apk add ncurses-dev
```
- For Slackware:
```bash
sudo slackpkg install ncurses
```
- For Gentoo
```bash
sudo emerge sys-libs/ncurses
```
- Linux-based OS (as it reads from `/proc`)

## Installation

### 1. Clone the Repository

```bash
git clone https://github.com/Hash-Cracker/ptop-for-linux
cd system-monitor
```
