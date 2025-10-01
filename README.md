# fw13-auto-brightness

### **fw13-auto-brightness** is a system for automatic laptop screen brightness control based on ambient light sensor data.

### The project consists of two parts:

- **AutoBrightnessIluminance** — a C++/DBus daemon running in the background and managing screen brightness.
- **AutoBrightnessUI** — a Qt-based application for configuring the daemon parameters through a graphical interface.


## Screenshots

<img src="screenshots/main_ui.png">
</p>

<div style="display:flex;">
<svg xmlns="http://www.w3.org/2000/svg" height="24px" viewBox="0 -960 960 960" width="24px" fill="#F19E39"><path d="M600-40q-33 0-56.5-23.5T520-120q0-23 11-41t29-29v-221q-18-11-29-28.5T520-480q0-33 23.5-56.5T600-560q33 0 56.5 23.5T680-480q0 23-11 40.5T640-411v115l160-53v-62q-18-11-29-28.5T760-480q0-33 23.5-56.5T840-560q33 0 56.5 23.5T920-480q0 23-11 40.5T880-411v119l-240 80v22q18 11 29 29t11 41q0 33-23.5 56.5T600-40ZM160-160v-560 560Zm0 0q-33 0-56.5-23.5T80-240v-480q0-33 23.5-56.5T160-800h240l80 80h320q33 0 56.5 23.5T880-640H447l-80-80H160v480h280v80H160Z"/></svg> <h3 style="margin: auto .5em;">Project structure:</h3>
</div>
</p>

```bash
fw13-auto-brightness
├── demon/        # sources of AutoBrightnessIluminance
├── ui/           # sources of the Qt UI (AutoBrightnessUI)
├── installer/    # installer, configs, icons, .desktop/.service files
└── README.md
```

## Development environment

The project was originally developed on Arch Linux (KDE Plasma).

The daemon uses systemd/*.h for DBus/systemd integration.

The UI is built on top of Qt6.

For a full list of required libraries, check
[demon/CMakeLists.txt](/demon/CMakeLists.txt) and [ui/CMakeLists.txt](/ui/CMakeLists.txt)

<div style="display:flex;"><svg xmlns="http://www.w3.org/2000/svg" height="36px" viewBox="0 -960 960 960" width="36px" fill="#F19E39"><path d="m230.67-490.67-8-52.66q-17.34-5-32.5-13.17Q175-564.67 162-576.67l-48.67 18L80-614l40-36.67q-4-17.66-4-34.66 0-17 4-34.67l-40-36.67L113.33-812 162-794q13-12 28.17-20.17 15.16-8.16 32.5-13.16l8-52.67h66.66l8 52.67q17.34 5 32.5 13.16Q353-806 366-794l48.67-18L448-756.67 407.33-720q4 17 4 34.33 0 17.34-4 35L448-614l-33.33 55.33-48.67-18q-13 12.67-28.17 20.5-15.16 7.84-32.5 12.84l-8 52.66h-66.66ZM264-597.33q36.33 0 62.17-25.84Q352-649 352-685.33q0-36.34-25.83-62.17-25.84-25.83-62.17-25.83t-62.17 25.83Q176-721.67 176-685.33q0 36.33 25.83 62.16 25.84 25.84 62.17 25.84ZM626.67-40l-15.34-66q-23.66-6.67-44.83-18.83-21.17-12.17-37.83-30.5l-62.67 20-44-75.34 49.33-44q-6-23.33-6-48 0-24.66 6-48L422-394l44-75.33 62.67 18.66q17.33-17.66 38.16-30.16 20.84-12.5 44.5-18.5l15.34-66H714l15.33 66q25 4.66 45.5 17.5 20.5 12.83 37.84 31.16l62.66-18.66 44 75.33L870-350.67q6 23.34 6 48 0 24.67-6 48l49.33 44-44 76-62.66-20.66q-16.67 19-37.84 30.83-21.16 11.83-45.5 18.5L714-40h-87.33Zm44-130q55.33 0 94-38.67 38.66-38.66 38.66-94 0-55.33-38.66-94-38.67-38.66-94-38.66-55.34 0-94 38.66Q538-358 538-302.67q0 55.34 38.67 94 38.66 38.67 94 38.67Z"/></svg>
<h3 style="margin: 0.1 .5em">Build</h3>
</div>


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

## Installation
<div style="display:flex"><svg xmlns="http://www.w3.org/2000/svg" height="24px" viewBox="0 -960 960 960" width="24px" fill="#F19E39"><path d="m40-120 440-760 440 760H40Zm138-80h604L480-720 178-200Zm302-40q17 0 28.5-11.5T520-280q0-17-11.5-28.5T480-320q-17 0-28.5 11.5T440-280q0 17 11.5 28.5T480-240Zm-40-120h80v-200h-80v200Zm40-100Z"/></svg> 
<p style="margin: auto .5em;">Requires administrator privileges!</p>
</div>

    cd installer
    sudo ./i.sh /opt

The script will:
- Copy files to /opt/autobrightnessiluminance
- Install autobrightness.desktop and autobrightness.service
- Automatically start the daemon via systemd

**Note**: _the daemon starts immediately after autobrightness.service is installed!service!_

# Contributing
### PRs and bug reports are welcome ;)

Planned:
 - Add more settings to the UI
 - Extend support for other DEs (Gnome, XFCE)
 - Test across different laptops

## Developer

- **Name**: Tymofii
- **GitHub**: [Ch-Tima](https://github.com/Ch-Tima)

<div style="display:flex;"> 
<svg xmlns="http://www.w3.org/2000/svg" height="24px" viewBox="0 -960 960 960" width="24px" fill="#EA3323"><path d="M440-280h80v-240h-80v240Zm40-320q17 0 28.5-11.5T520-640q0-17-11.5-28.5T480-680q-17 0-28.5 11.5T440-640q0 17 11.5 28.5T480-600Zm0 520q-139-35-229.5-159.5T160-516v-244l320-120 320 120v244q0 152-90.5 276.5T480-80Zm0-84q104-33 172-132t68-220v-189l-240-90-240 90v189q0 121 68 220t172 132Zm0-316Z"/></svg>
<h2 style="margin: auto .25em;">License</h2>
</div>

This project is licensed under the GNU Affero General Public License v3.0 (AGPL-3.0)  
See the [LICENSE](LICENSE) file for details.
