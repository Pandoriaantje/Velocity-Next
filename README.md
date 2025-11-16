Velocity-Next
=============

**Velocity-Next is a continuation of the original [Velocity](https://github.com/hetelek/Velocity) project, which ceased active development some time ago. This fork aims to modernize the codebase, add new features, and maintain compatibility with current operating systems and toolchains.**

### What's New in Velocity-Next

- **Smart Pointer Refactoring**: Modern C++20 memory management for improved stability
- **ISO/XEX Integration**: Merged improvements from Gualdimar/Velocity fork
- **MSVC 2022 Support**: Full compatibility with Visual Studio 2022 alongside MinGW
- **Modernized Build System**: CMake 3.20+ with modern target-based patterns
- **Standalone XboxInternals Library**: Can now be distributed and used independently
- **Active Development**: Issues and PRs are welcomed and will be addressed

### Credits

Original Velocity was developed mainly by Stevie Hetelekides (Hetelek) and Adam Spindler (Experiment5X), with contributions from [many others](https://github.com/hetelek/Velocity/graphs/contributors). Velocity-Next continues their work with gratitude and respect for the foundation they built.

## Important: Administrator Privileges Required (Windows)

On **Windows**, Velocity requires **Administrator privileges** to access Xbox 360 drives:

## Running Velocity-Next

- **Right-click** `VelocityNext.exe` → Select **"Run as administrator"**
- This is necessary for detecting and browsing Xbox 360 hard drives and USB devices
- Without Administrator rights, drives will **not be detected** in the Device Viewer

See [BUILD.md](BUILD.md#running-velocity) for Linux/macOS requirements and more details.


About Velocity-Next
-------------------
Velocity-Next is a cross-platform Xbox 360 file management application built with Qt 6. Browse, edit, and extract content from Xbox 360 hard drives, USB devices, game saves, profiles, and disc images.

### Supported File Formats

- **FATX** - Xbox 360 filesystem (hard drives, USB devices)
- **STFS** - Game saves, profiles, DLC packages (CON/LIVE/PIRS)
- **GPD** - Gamer profile data (achievements, avatar awards, settings)
- **XDBF** - Xbox database files
- **ISO** - Disc images (GDF, XGD3, Redump formats) with file extraction
- **XEX** - Xbox executables with metadata parsing
- **SVOD** - Secure Video on Demand packages
- **YTGR** - Avatar asset packages

### International Text Support

View text files from Japanese, Chinese, and Korean game discs with automatic encoding detection:
- Japanese (Shift-JIS), Simplified Chinese (GBK), Traditional Chinese (Big5), Korean (EUC-KR)
- UTF-8, UTF-16, Latin-1
- Manual encoding override available

Licensing Information
---------------------
Velocity-Next is licensed and distributed under the GNU General Public License (v3), continuing the license of the original Velocity project.

Velocity-Next is free software: you can redistribute it and/or modify it under the terms of the GNU General Public License as published by the Free Software Foundation, either version 3 of the License, or (at your option) any later version.

If no copy of the GNU General Public License was received with this program (FILE: COPYING), it is available at <http://www.gnu.org/licenses/>.

### Third-Party Components

Velocity-Next includes and statically links the following third-party libraries:

- **Botan** (BSD-2-Clause): Cryptography library embedded as amalgamation build. See [LICENSE-Botan.txt](LICENSE-Botan.txt) for full license text.
- **Qt 6**: Cross-platform application framework (LGPL v3 / GPL v3). Qt is dynamically linked (MSVC builds) or statically linked (MinGW builds).

### Build System Acknowledgments

- Botan CMake integration adapted from [Tectu/botan-cmake](https://github.com/Tectu/botan-cmake)
