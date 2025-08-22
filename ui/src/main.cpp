#include <QApplication>
#include <QWidget>
#include <QPushButton>
#include <QVBoxLayout>
#include <QHBoxLayout>
#include <QSize>
#include <QLabel>
#include <QLineEdit>
#include <QFormLayout>
#include <QSize>
#include <QtCharts/QChart>
#include <QtCharts/QLineSeries>
#include <QtCharts/QChartView>
#include <QTableWidget>

#include <QGridLayout>

#include <QDBusInterface>
#include <QDBusConnection>
#include <QDBusReply>

#include <QTimer>
#include <QObject>

#include <vector>
#include <iostream>
#include <chrono>

struct vec2_u32
{
    u_int16_t il;
    uint16_t br;
};

int main(int argc, char *argv[]){

    QApplication app(argc, argv);
    QWidget window;
    window.setWindowTitle("Auto Brightness Settings");
    window.setMinimumSize(QSize(900, 600));
    
    QVBoxLayout *layout = new QVBoxLayout(&window);

    //BEGIN_MAIN_LAYOUT

    QGridLayout *main_layout = new QGridLayout();
    
    QFormLayout *form = new QFormLayout();

    QLineEdit *input_limit = new QLineEdit();
    input_limit->setPlaceholderText("0 - 255 (default: 50)");
    input_limit->setFixedSize(225, 30);

    QLineEdit *input_check = new QLineEdit();
    input_check->setPlaceholderText("0 - 255 (default: 3)");
    input_check->setFixedSize(225, 30);

    QLineEdit *input_time = new QLineEdit();
    input_time->setPlaceholderText("0.1 - 60 sec (default: 0.5)");
    input_time->setFixedSize(225, 30);

    form->addRow("New limit:", input_limit);
    form->addRow("Number of checks:", input_check);
    form->addRow("LoopDelayMs:", input_time);

    main_layout->addLayout(form, 0, 0, 1, 1);
    main_layout->setRowStretch(0, 0);  // форма без растяжки


    QLineSeries* series = new QLineSeries();
    std::vector<vec2_u32> v = {
        { 0,    500   }, 
        { 20,   3000  }, 
        { 80,   4000  }, 
        { 100,  5000  }, 
        { 200,  5500  },   
        { 300,  6000  },   
        { 500,  7000  },
        { 1400, 8500  },
        { 3355, 10000 }
    };
    
    for(vec2_u32 item : v){
        series->append(item.br, item.il);
    }
    
    //QChart
    QChart *chart = new QChart();
    chart->addSeries(series);
    QChartView *chartView = new QChartView(chart);
    main_layout->addWidget(chartView, 0, 1, 2, 1);
    

    //QTableWidget
    QTableWidget *table = new QTableWidget(0, 2);
    table->setHorizontalHeaderLabels({"Value", "Brightness`%`"});
    for (vec2_u32 item : v) {
        int row = table->rowCount();
        table->insertRow(row);

        QLineEdit *edit1 = new QLineEdit();
        edit1->setText(QString::number(item.il));
        QLineEdit *edit2 = new QLineEdit();
        edit2->setText(QString::number(item.br/100));
        table->setCellWidget(row, 0, edit1);
        table->setCellWidget(row, 1, edit2);
    }

    main_layout->addWidget(table, 1, 0, 2, 1);
    main_layout->setRowStretch(1, 1);  // таблица растягивается

    //Current il_value
    QLabel *current_il_value = new QLabel("ilum:1475"); 
    main_layout->addWidget(current_il_value, 2, 1, 1, 1);
    //END_MAIN_LAYOUT


    QHBoxLayout *bottom_btn_layout = new QHBoxLayout();
    bottom_btn_layout->addStretch();
    QPushButton *btn_cancel = new QPushButton("Cancel");
    btn_cancel->setFixedSize(100, 30);
    bottom_btn_layout->addWidget(btn_cancel/*, 0, Qt::AlignRight */);
    QPushButton *btn_applay = new QPushButton("Aplay");
    btn_applay->setFixedSize(100, 30);
    bottom_btn_layout->addWidget(btn_applay);


    layout->addLayout(main_layout);
    layout->addLayout(bottom_btn_layout);
    QDBusInterface interface(
        "com.ct.AutoBrightness",    // service name (bus name, тот что в --dest)
        "/com/ct/AutoBrightness",   // object path (тот что в dbus-send после dest)
        "com.ct.AutoBrightness",    // interface (тот что в dbus-send после object path)
        QDBusConnection::sessionBus()
    );

    
    if(!interface.isValid()){
        std::cout << "ERR" << std::endl;
    }

    QTimer *timer = new QTimer(&window);
    QObject::connect(timer, &QTimer::timeout, [&]() {
        QDBusReply<uint16_t> reply = interface.call("GetIlluminance");
        if (reply.isValid()){
            std::cout << reply.value() << std::endl;
            current_il_value->setText("ilum:" + QString::number(reply.value()));
        }else{
            current_il_value->setText("NO SIGNAL!");
        }
    });
    timer->start(1000);

    QDBusReply<uint16_t> reply = interface.call("GetLoopDelayMs");
    if(reply.isValid()){
        input_time->setText(QString::number(reply.value()));
    }else{
        input_time->setText("NO SIGNAL!");
    }


    window.show();
    return app.exec();
}


/*

new_limit : 0 - 255 (деопозон новезны) d:50
number_of_check : 0 - 255 (количетво проверок на новезну значения ’il_value’) d:3
time : 0.1 до 60 (время между повторным побором значения для ’il_value’) d:500ms OR 0.5s
vec2_u32 v[size] (масси значения для сопостовления ’il_value’ с яркостью экрана)
  

*/