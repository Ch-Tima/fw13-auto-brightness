#!/bin/bash
#V2
if [ $(whoami) != 'root' ]; then
        echo "Please use 'sudo' to run me!"
        exit -1;
fi

REAL_USER=${SUDO_USER:-$USER}
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

mkdir -p "${WHERE_INSTALL}/bin"
mkdir -p "${WHERE_INSTALL}/res"

echo "Install AutoBrightnessUI"
cp AutoBrightnessUI "${WHERE_INSTALL}/bin"
chmod 755 "${WHERE_INSTALL}/bin/AutoBrightnessUI" 
cp aib.conf "${WHERE_INSTALL}/bin"
chmod 644 "${WHERE_INSTALL}/bin/aib.conf" 

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