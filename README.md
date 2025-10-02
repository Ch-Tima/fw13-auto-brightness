# fw13-auto-brightness

<img src="readme/icons/icon.svg" align="left" width="50" style="padding-right: .75em;" />

### **fw13-auto-brightness** is a system for automatic laptop screen brightness control based on ambient light sensor data.


### The project consists of two parts:

- **AutoBrightnessIluminance** — a C++/DBus daemon running in the background and managing screen brightness.
- **AutoBrightnessUI** — a Qt-based application for configuring the daemon parameters through a graphical interface.


<h2>
<img src="readme/icons/screenshot.svg"> Screenshots
</h2>

<img src="screenshots/main_ui.png">
</p>

<h2 style="">
<img src="readme/icons/dir_structure.svg">
Project structure:
</h2>
</p>

```bash
fw13-auto-brightness
├── demon/        # sources of AutoBrightnessIluminance
├── ui/           # sources of the Qt UI (AutoBrightnessUI)
├── installer/    # installer, configs, icons, .desktop/.service files
└── README.md
```

<h2>
<img src="readme/icons/device.svg">
Development environment
</h2>

The project was originally developed on Arch Linux (KDE Plasma).

The daemon uses systemd/*.h for DBus/systemd integration.

The UI is built on top of Qt6.

For a full list of required libraries, check
[demon/CMakeLists.txt](/demon/CMakeLists.txt) and [ui/CMakeLists.txt](/ui/CMakeLists.txt)

<h2>
<img src="readme/icons/settings.svg">
Build
</h2>
</p>

    Requirements:
    - CMake ≥ 3.10
    - Qt6 (Core, Widgets, DBus, Charts, Svg, Concurrent)
    - systemd headers/INIReader

```bash
#UI
cd ui
mkdir build && cd build
cmake ..
make
./AutoBrightnessUI

#SERVICE
cd demon
mkdir build && cd build
cmake ..
make
./AutoBrightnessIluminance

```

<h2><img src="readme/icons/arrow_d.svg"> Installation</h2>
<div style="display:flex">
<img src="readme/icons/attention.svg"/>
<p style="margin: auto .5em;">Requires administrator privileges!</p>
</div>

    cd installer
    sudo ./i.sh /opt

The script will:
- Copy files to /opt/autobrightnessiluminance
- Install autobrightness.desktop and autobrightness.service
- Automatically start the daemon via systemd

**Note**: _the daemon starts immediately after autobrightness.service is installed!service!_

<h2>
<img src="readme/icons/construction.svg">
Contributing
</h2>

### PRs and bug reports are welcome ;)

Planned:
 - Add more settings to the UI
 - Extend support for other DEs (Gnome, XFCE)
 - Test across different laptops

<h2>
<img src="readme/icons/code.svg"> Developer
</h2>

- **Name**: Tymofii
- **GitHub**: [Ch-Tima](https://github.com/Ch-Tima)

<h2>
<img src="readme/icons/license.svg">
License
</h2>

This project is licensed under the GNU Affero General Public License v3.0 (AGPL-3.0)  
See the [LICENSE](LICENSE) file for details.
