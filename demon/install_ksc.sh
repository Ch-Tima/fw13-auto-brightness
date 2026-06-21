#/bin/bash

kpackagetool6 --type=KWin/Script -i kwin_active_window
kwriteconfig6 --file kwinrc --group Plugins --key ABI_GetActWinEnabled true
qdbus6 org.kde.KWin /KWin reconfigure
journalctl --user -f | grep -i kwin
