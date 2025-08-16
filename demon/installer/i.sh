#!/bin/bash

if [ $(whoami) != 'root' ]; then
        echo "Please use 'sudo' to run me!"
        exit -1;
fi

REAL_USER=${SUDO_USER:-$USER}
WHERE_INSTALL="${1}/autobrightnessiluminance"
FILE_SERVICE=""

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

echo "install program"
> /dev/null
mkdir $WHERE_INSTALL
cp AutoBrightnessIluminance $WHERE_INSTALL
chmod 551 "${WHERE_INSTALL}/AutoBrightnessIluminance" 

echo "install .service"
echo "$FILE_SERVICE" | sudo tee -a /etc/systemd/system/autobrightness.service > /dev/null

echo "start .service"
sudo systemctl daemon-reload
sudo systemctl enable autobrightness.service
sudo systemctl start autobrightness.service
systemctl status autobrightness.service

echo "END :)"

exit 0


