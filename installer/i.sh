#!/bin/bash
# fw13-auto-brightness
# Part of fw13-auto-brightness project (installer scripts)
# Copyright (C) 2025  <Ch-Tima>
#
# This program is free software: you can redistribute it and/or modify
# it under the terms of the GNU Affero General Public License as published by
# the Free Software Foundation, either version 3 of the License, or
# (at your option) any later version.
#
# This program is distributed in the hope that it will be useful,
# but WITHOUT ANY WARRANTY; without even the implied warranty of
# MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.
#
# See the GNU Affero General Public License for more details:
# <https://www.gnu.org/licenses/>.
#V2
if [ $(whoami) != 'root' ]; then
        echo "Please use 'sudo' to run me!"
        exit -1;
fi

REAL_USER=${SUDO_USER:-$USER}
USER_ID=$(id -u "$REAL_USER")
WHERE_INSTALL="${1}/autobrightnessiluminance"
FILE_SERVICE=""
FILE_DESKTOP=""

echo $WHERE_INSTALL

echo "create .service"

while read -r line; do
    if [[ $line == *"?U"* ]];then
        line="${line/'?'U/"$REAL_USER"}"
    fi

    if [[ $line == *"?ES"* ]];then 
        line="${line/'?'ES/"$WHERE_INSTALL"/AutoBrightnessIluminance}"
    fi

    if [[ $line == *"?WD"* ]];then 
        line="${line/'?'WD/"$WHERE_INSTALL"}"
    fi

    if [[ $line == *"?ID"* ]]; then
        line="${line/'?'ID/"$USER_ID"}"
    fi

    FILE_SERVICE="${FILE_SERVICE}${line}"$'\n'
done < e.service

echo "install .service"
mkdir -p $WHERE_INSTALL
cp AutoBrightnessIluminance $WHERE_INSTALL
chmod 755 "${WHERE_INSTALL}/AutoBrightnessIluminance" 

echo "$FILE_SERVICE" | sudo tee /etc/systemd/system/autobrightness.service > /dev/null

echo "start .service"
sudo systemctl daemon-reload
sudo systemctl enable autobrightness.service
sudo systemctl start autobrightness.service
systemctl is-active --quiet autobrightness.service && echo "Service started successfully"

#MAKE DIR
mkdir -p "${WHERE_INSTALL}/bin"
mkdir -p "${WHERE_INSTALL}/res"
mkdir -p "/home/${REAL_USER}/.config/autobrightness/"
#BIN UI
echo "Install AutoBrightnessUI"
cp AutoBrightnessUI "${WHERE_INSTALL}/bin"
chmod 755 "${WHERE_INSTALL}/bin/AutoBrightnessUI" 
#CONFIG
cp aib.conf "/home/${REAL_USER}/.config/autobrightness"
chmod 644 "/home/${REAL_USER}/.config/autobrightness/aib.conf" 
#RES
echo "Copy res"
cp icon.png "${WHERE_INSTALL}/res"
chmod 755 "${WHERE_INSTALL}/res/icon.png" 
cp icon.svg "${WHERE_INSTALL}/res"
chmod 755 "${WHERE_INSTALL}/res/icon.svg" 

echo "create .desktop"
while read -r line; do

    if [[ $line == *"?EP"* ]];then
        line="${line/'?'EP/"$WHERE_INSTALL"/bin/AutoBrightnessUI}"
    fi
    if [[ $line == *"?IC"* ]];then
        line="${line/'?'IC/"$WHERE_INSTALL"/res/icon.png}"
    fi
    FILE_DESKTOP="${FILE_DESKTOP}${line}"$'\n'
done < d.desktop

echo "$FILE_DESKTOP" | sudo tee /usr/share/applications/autobrightness.desktop > /dev/null

echo "END :)"

exit 0

# Ch-Tima