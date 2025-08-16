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
#include <vector>

#include <iostream>

struct vec2_u32
{
    double x;
    double y;
};

int main(int argc, char *argv[]){

    QApplication app(argc, argv);
    QWidget window;
    window.setWindowTitle("Auto Brightness Settings");
    window.setMinimumSize(QSize(900, 600));
    
    QVBoxLayout *layout = new QVBoxLayout(&window);

    //BEGIN_MAIN_LAYOUT

    QHBoxLayout *main_layout = new QHBoxLayout();
    

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
    form->addRow("Time interval:", input_time);

    main_layout->addLayout(form);


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
        series->append(item.y, item.x);
    }
    
    QChart *chart = new QChart();
    chart->addSeries(series);
    QChartView *chartView = new QChartView(chart);
    main_layout->addWidget(chartView);


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


    window.show();
    return app.exec();
}


/*

new_limit : 0 - 255 (деопозон новезны) d:50
number_of_check : 0 - 255 (количетво проверок на новезну значения ’il_value’) d:3
time : 0.1 до 60 (время между повторным побором значения для ’il_value’) d:500ms OR 0.5s
vec2_u32 v[size] (масси значения для сопостовления ’il_value’ с яркостью экрана)
  

*/