#include "view/MainView.h"


MainView::MainView(QWidget *parent){
    init();
    dbus = new DbusClient(this);
    //тут обрабатуем запрос снизу
    connect(dbus, &DbusClient::illuminanceReceived, this, [&](short value){
        if(value < 0){
            current_il_value->setText("NO SIGNAL!");
        }else{
            current_il_value->setText("ilum:" + QString::number(value));
        }
    }); 

    QTimer *timer = new QTimer(this);//тут запрашиваем 
    connect(timer, &QTimer::timeout, dbus, &DbusClient::requestIlluminance);
    timer->start(1000);

    connect(dbus, &DbusClient::loopDelayMsReceived, this, [&](short value){
        if(value < 0){
            input_time->setText("NO SIGNAL!");
        }else{
            input_time->setText(QString::number(value));
        }
    });

    connect(dbus, &DbusClient::THRReceived, this, [&](short value){
        if(value < 0){
            input_limit->setText("NO SIGNAL!");
        }else{
            input_limit->setText(QString::number(value));
        }
    });

    connect(dbus, &DbusClient::validationCountReceived, this, [&](short value){
        if(value < 0){
            input_check->setText("NO SIGNAL!");
        }else{
            input_check->setText(QString::number(value));
        }
    });

    connect(dbus, &DbusClient::pointsReceived, this, [&](const Vec2List &points){
        series->clear();
        if (points.isEmpty()) {
            qDebug() << "Нет данных от DBus";
        } else {
            qDebug() << "Получено" << points.size() << "точек";
            for (vec2_u16 item : points) {
                int row = table->rowCount();
                table->insertRow(row);
        
                QLineEdit *edit1 = new QLineEdit();
                edit1->setText(QString::number(item.il));
                QLineEdit *edit2 = new QLineEdit();
                edit2->setText(QString::number(item.br/100));
                table->setCellWidget(row, 0, edit1);
                table->setCellWidget(row, 1, edit2);
                series->append(item.br/100, item.il);
            }
            chart->update();
        }
    });

    dbus->requestPoints();
    dbus->requestValidationCount();
    dbus->requestTHR();
    dbus->requestLoopDelayMs();
}

int MainView::init(){
    setWindowTitle("Auto Brightness Settings");
    setMinimumSize(QSize(900, 600));


    layout = new QVBoxLayout(this);

    //BEGIN_MAIN_LAYOUT
    main_layout = new QGridLayout();

    form = new QFormLayout();
    
    //inputs
    input_limit = new QLineEdit();
    input_limit->setPlaceholderText("0 - 255 (default: 50)");
    input_limit->setFixedSize(225, 30);

    input_check = new QLineEdit();
    input_check->setPlaceholderText("0 - 255 (default: 3)");
    input_check->setFixedSize(225, 30);

    input_time = new QLineEdit();
    input_time->setPlaceholderText("0.1 - 60 sec (default: 0.5)");
    input_time->setFixedSize(225, 30);

    form->addRow("New limit:", input_limit);
    form->addRow("Number of checks:", input_check);
    form->addRow("LoopDelayMs:", input_time);


    main_layout->addLayout(form, 0, 0, 1, 1);
    main_layout->setRowStretch(0, 0);  // форма без растяжки


    //QChart
    chart = new QChart();
    series = new QLineSeries();
    chart->addSeries(series);
    // создаём оси
    QValueAxis *axisX = new QValueAxis();
    QValueAxis *axisY = new QValueAxis();
    axisX->setTitleText("Brightness %");
    axisY->setTitleText("Illuminance");
    axisX->setRange(0, 100);
    axisY->setRange(0, 4059);
    chart->addAxis(axisX, Qt::AlignBottom);
    chart->addAxis(axisY, Qt::AlignLeft);
    series->attachAxis(axisX);
    series->attachAxis(axisY);
    series->setPointsVisible(true);
    chartView = new QChartView(chart);
    main_layout->addWidget(chartView, 0, 1, 2, 1);

    //QTableWidget
    table = new QTableWidget(0, 2);
    table->setHorizontalHeaderLabels({"Value", "Brightness`%`"});

    main_layout->addWidget(table, 1, 0, 2, 1);
    main_layout->setRowStretch(1, 1);  // таблица растягивается


    //Current il_value
    current_il_value = new QLabel("ilum:1475"); 
    main_layout->addWidget(current_il_value, 2, 1, 1, 1);
    //END_MAIN_LAYOUT

    //Buttons
    bottom_btn_layout = new QHBoxLayout();
    bottom_btn_layout->addStretch();
    btn_cancel = new QPushButton("Cancel");
    btn_cancel->setFixedSize(100, 30);
    bottom_btn_layout->addWidget(btn_cancel/*, 0, Qt::AlignRight */);
    btn_applay = new QPushButton("Aplay");
    btn_applay->setFixedSize(100, 30);
    bottom_btn_layout->addWidget(btn_applay);

    //Add all
    layout->addLayout(main_layout);
    layout->addLayout(bottom_btn_layout);
    return 0;
};