print("SCRIPT START");

for (let k in workspace) {
    print(k);
}


workspace.windowActivated.connect(function(win) {
    print("ACTIVATED_5");

    if(win.resourceClass == "org.kde.plasmashell" || win.resourceName == "org.kde.plasmashell"){
        print("return");
        return;
    }

    if (win) {
        print("class        = " + win.resourceClass);
        callDBus(
            "com.ct.AutoBrightness",
            "/com/ct/AutoBrightness",
            "com.ct.AutoBrightness",
            "GiveMeActiveWin",
            win.resourceClass
        );
    } else {
        print("NULL WINDOW");
    }
});
