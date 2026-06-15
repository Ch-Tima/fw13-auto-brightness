print("SCRIPT START");

for (let k in workspace) {
    print(k);
}


workspace.windowActivated.connect(function(win) {
    print("ACTIVATED_4");

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
