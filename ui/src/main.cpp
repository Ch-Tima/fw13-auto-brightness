#include <QApplication>
#include <QWidget>
#include "view/MainView.h"


int main(int argc, char *argv[]){

    QApplication app(argc, argv);
    MainView window;
    window.show();
    return app.exec();
}


/*

new_limit : 0 - 255 (деопозон новезны) d:50
number_of_check : 0 - 255 (количетво проверок на новезну значения ’il_value’) d:3
time : 0.1 до 60 (время между повторным побором значения для ’il_value’) d:500ms OR 0.5s
vec2_u32 v[size] (масси значения для сопостовления ’il_value’ с яркостью экрана)
  

*/