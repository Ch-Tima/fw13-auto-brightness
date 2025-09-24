#include <QApplication>
#include <QWidget>
#include "view/MainView.h"


int main(int argc, char *argv[]){

    QApplication app(argc, argv);
    MainView window;
    window.show();
    return app.exec();
}