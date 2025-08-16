#include <QApplication>
#include <QWidget>

int main(int argc, char *argv[]){

    QApplication app(argc, argv);
    QWidget win;
    win.setWindowTitle("Auto Brightness Settings");

    win.show();
    return app.exec();
}