#include "view/MainView.h"


MainView::MainView(QWidget *parent){
    init();
    dbus = new DbusClient(this);
    //тут обрабатуем запрос снизу
    connect(dbus, &DbusClient::illuminanceReceived, this, [&](short value){
        if(value < 0){//"NO SIGNAL!"
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
            input_loop_delay->setValue(origConfig.loopDelayMs);
        }else{
            origConfig.loopDelayMs = value;
            input_loop_delay->setValue(origConfig.loopDelayMs);
        }
    });

    connect(dbus, &DbusClient::THRReceived, this, [&](short value){
        if(value < 0){
            input_change_threshold->setValue(origConfig.changeThreshold);
        }else{
            origConfig.changeThreshold = value;
            input_change_threshold->setValue(origConfig.changeThreshold);
        }
    });

    connect(dbus, &DbusClient::validationCountReceived, this, [&](short value){
        if(value < 0){
            input_validation_count->setValue(origConfig.validationCount);
        }else{
            origConfig.validationCount = value;
            input_validation_count->setValue(origConfig.validationCount);
        }
    });

    connect(dbus, &DbusClient::pointsReceived, this, [&](const Vec2List &points){
        series->clear();
        if (points.isEmpty()) {
            qDebug() << "Нет данных от DBus";
        } else {
            qDebug() << "Получено" << points.size() << "точек";
            for (vec2_u16 item : points) {
                origConfig.brakePoints.push_back(item);
                int row = table->rowCount();
                table->insertRow(row);
        
                QSpinBox *edit1 = new QSpinBox();
                edit1->setMaximum(MAX_VALUE_12BIT_ADC);
                connect(edit1, &QSpinBox::valueChanged, this, &MainView::checkChangesWithConfig);
                QSpinBox *edit2 = new QSpinBox();
                edit2->setMaximum(100);
                connect(edit2, &QSpinBox::valueChanged, this, &MainView::checkChangesWithConfig);

                edit1->setValue(item.il);
                edit2->setValue(item.br/100);

                table->setCellWidget(row, 0, edit1);
                table->setCellWidget(row, 1, edit2);
                series->append(item.br/100, item.il);
            }
            chart->update();
            checkChangesWithConfig();
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
    input_change_threshold = new QSpinBox();
    input_change_threshold->setFixedSize(225, 30);
    input_change_threshold->setMaximum(__UINT16_MAX__);
    connect(input_change_threshold, &QSpinBox::valueChanged, this, &MainView::checkChangesWithConfig);


    input_validation_count = new QSpinBox();
    input_validation_count->setFixedSize(225, 30);
    input_validation_count->setMaximum(__UINT8_MAX__);
    connect(input_validation_count, &QSpinBox::valueChanged, this, &MainView::checkChangesWithConfig);


    input_loop_delay = new QSpinBox();
    input_loop_delay->setFixedSize(225, 30);
    input_loop_delay->setMaximum(60000);//60sec == 60000ms
    input_loop_delay->setSuffix("ms");
    connect(input_loop_delay, &QSpinBox::valueChanged, this, &MainView::checkChangesWithConfig);


    form->addRow("change threshold:", input_change_threshold);
    form->addRow("validation count:", input_validation_count);
    form->addRow("loop delay:", input_loop_delay);


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
    axisY->setRange(0, MAX_VALUE_12BIT_ADC);
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
    
    svg_update = new QSvgWidget("../assets/update_24dp.svg");
    svg_update->setSizePolicy(QSizePolicy::Fixed, QSizePolicy::Fixed);
    svg_update->hide();
    bottom_btn_layout->addWidget(svg_update, Qt::AlignCenter);

    svg_update = new QSvgWidget("../assets/check_circle_24dp.svg");
    svg_update->setSizePolicy(QSizePolicy::Fixed, QSizePolicy::Fixed);
    svg_update->show();
    bottom_btn_layout->addWidget(svg_update, Qt::AlignCenter);

    btn_cancel = new QPushButton("Cancel");
    btn_cancel->setFixedSize(100, 30);
    bottom_btn_layout->addWidget(btn_cancel/*, 0, Qt::AlignRight */);
    btn_applay = new QPushButton("Aplay");
    btn_applay->setFixedSize(100, 30);
    btn_applay->setEnabled(false);
    bottom_btn_layout->addWidget(btn_applay);

    //Add all
    layout->addLayout(main_layout);
    layout->addLayout(bottom_btn_layout);

    checkChangesWithConfig();

    return 0;
};

int MainView::convertToValidNumber(const QString &text, int min, int max){
    if(text[0] == '0'){
        return 1;
    }
    bool ok; 
    int value = text.toInt(&ok);
    if (!ok || value < min || value >= max) {
        return 255;
    }
    return value;
}

void MainView::checkChangesWithConfig(){
    bool result = false;

    //Проверяет простые поля
    if(input_change_threshold->value() != origConfig.changeThreshold || 
    input_loop_delay->value() != origConfig.loopDelayMs ||
    input_validation_count->value() != origConfig.validationCount){
        result = true;
    }
    //Проверяет таблицу
    if(!result){
        //если удолена или новая запись
        if (table->rowCount() != static_cast<int>(origConfig.brakePoints.size())) {
            result = true;
        }else{//проверка значений в таблице
            for(int row = 0; row < table->rowCount(); row++){
                auto *editIl = qobject_cast<QSpinBox*>(table->cellWidget(row, 0));
                auto *editBr = qobject_cast<QSpinBox*>(table->cellWidget(row, 1));

                if (!editIl || !editBr) continue;

                int il = editIl->value();
                int br = editBr->value();

                if (il != origConfig.brakePoints[row].il ||
                    br != origConfig.brakePoints[row].br / 100) {
                    result = true;
                    break;
                }
            }
        }
    }

    btn_applay->setEnabled(result);
}