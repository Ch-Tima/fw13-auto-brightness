#include "view/MainView.h"


MainView::MainView(QWidget *parent) : QWidget(parent), uiBlocked(false){
    init();
    this->installEventFilter(this);
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
                insertNewPointToTable(item.il, item.br/100);
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
    table = new QTableWidget(0, 3);
    table->setHorizontalHeaderLabels({"Ilum", "Brightness`%`", "x"});
    table->setEditTriggers(QAbstractItemView::AllEditTriggers);
    table->setSortingEnabled(true);

    main_layout->addWidget(table, 1, 0, 2, 1);
    main_layout->setRowStretch(1, 1);  // таблица растягивается

    //Current il_value
    current_il_value = new QLabel("ilum:1475"); 
    main_layout->addWidget(current_il_value, 2, 1, 1, 1);
    //END_MAIN_LAYOUT

    //Buttons
    bottom_btn_layout = new QHBoxLayout();
    bottom_btn_layout->addStretch();
    

    QGraphicsScene* scene = new QGraphicsScene(this);
    svg_update_item = new QGraphicsSvgItem("../assets/update_24dp.svg");
    svg_update_item->setFlags(QGraphicsItem::ItemClipsToShape);
    svg_update_item->setCacheMode(QGraphicsItem::NoCache);
    svg_update_item->setZValue(0);

    scene->addItem(svg_update_item);

    svg_update_view = new QGraphicsView(scene, this);
    svg_update_view->setStyleSheet("background: transparent; border: none;");
    svg_update_view->setRenderHints(QPainter::Antialiasing | QPainter::SmoothPixmapTransform);
    svg_update_view->setFixedSize(32, 32); // под размер иконки
    svg_update_view->hide();

    svg_update_view->setAlignment(Qt::AlignCenter);
    svg_update_view->setResizeAnchor(QGraphicsView::AnchorViewCenter);
    svg_update_view->setSceneRect(svg_update_item->boundingRect());
    bottom_btn_layout->addWidget(svg_update_view, Qt::AlignCenter);

    // svg_update = new QSvgWidget("../assets/update_24dp.svg");
    // svg_update->setSizePolicy(QSizePolicy::Fixed, QSizePolicy::Fixed);
    // svg_update->hide();
    // bottom_btn_layout->addWidget(svg_update, Qt::AlignCenter);

    svg_ok = new QSvgWidget("../assets/check_circle_24dp.svg");
    svg_ok->setSizePolicy(QSizePolicy::Fixed, QSizePolicy::Fixed);
    svg_ok->show();
    bottom_btn_layout->addWidget(svg_ok, Qt::AlignCenter);


    btn_add_points = new QPushButton("AddPoint");
    btn_add_points->setFixedSize(100, 30);
    //btn_add_points->setIcon(QIcon("../assets/add_circle_24dp.svg"));
    //btn_add_points->setIconSize(QSize(32, 32));
    // btn_add_points->setStyleSheet(
    //     "QPushButton {"
    //         "border-radius: 28px;"
    //         "background-color: #272A2E;"
    //     "}"
    //     "QPushButton:hover {"
    //         "background-color: #292C30;" 
    //         "border: 1px solid #23A5D9;"
    //     "}"
    // );
    connect(btn_add_points, &QPushButton::released, this, [&](){insertNewPointToTable();});
    bottom_btn_layout->addWidget(btn_add_points);

    btn_cancel = new QPushButton("Cancel");
    btn_cancel->setFixedSize(100, 30);
    btn_cancel->setEnabled(false);
    bottom_btn_layout->addWidget(btn_cancel/*, 0, Qt::AlignRight */);
    btn_applay = new QPushButton("Aplay");
    btn_applay->setFixedSize(100, 30);
    btn_applay->setEnabled(false);
    bottom_btn_layout->addWidget(btn_applay);

    //Add all
    layout->addLayout(main_layout);
    layout->addLayout(bottom_btn_layout);

    checkChangesWithConfig();

    connect(btn_applay, &QPushButton::clicked, this, &MainView::applayConfigToDemon);
    connect(btn_cancel, &QPushButton::clicked, this, &MainView::resetFields);

    overlay = new QWidget(this);
    overlay->installEventFilter(this);
    overlay->setStyleSheet("background-color: rgba(0,0,0,85%);");
    overlay->setGeometry(this->rect());
    overlay->releaseKeyboard();
    overlay->raise();
    overlay->hide();

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
        qDebug() << "rowCount:" << table->rowCount();
        qDebug() << "brakePointsSize:" << static_cast<int>(origConfig.brakePoints.size());
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

    if(!result){
        svg_ok->show();
    }else svg_ok->hide();

    btn_applay->setEnabled(result);
    btn_cancel->setEnabled(result);
}

void MainView::applayConfigToDemon(){
    svg_ok->hide();
    //svg_update->show();
    btn_applay->setEnabled(false);
    btn_cancel->setEnabled(false); // блокируем кнопку

    if(input_loop_delay->value() != origConfig.loopDelayMs) {
        quint16 ld = static_cast<quint16>(input_loop_delay->value());
        qDebug() << "ld:" << ld;
        dbus->setLoopDelay(ld, [this, ld](bool ok, const QString &msg) {
            if(ok){
                qDebug() << "OK >> setLoopDelay";
                origConfig.loopDelayMs = ld;
            } else {
                qDebug() << "fail >> setLoopDelay. msg:" << msg;
            }
        });
    }

    if(input_validation_count->value() != origConfig.validationCount) {
        quint8 vc = static_cast<quint8>(input_validation_count->value());
        qDebug() << "vc:" << vc;
        dbus->setValidationCount(vc, [this, vc](bool ok, const QString &msg) {
            if(ok){
                qDebug() << "OK >> setValidationCount";
                origConfig.validationCount = vc;
            } else {
                qDebug() << "fail >> setValidationCount. msg:" << msg;
            }
        });
    }

    if(input_change_threshold->value() != origConfig.changeThreshold) {
        quint16 ch = static_cast<quint16>(input_change_threshold->value());
        qDebug() << "ch:" << ch;
        dbus->setChangeThreshold(ch, [this, ch](bool ok, const QString &msg) {
            if(ok){
                qDebug() << "OK >> SetChangeThreshold";
                origConfig.changeThreshold = ch;
            } else {
                qDebug() << "fail >> SetChangeThreshold. msg:" << msg;
            }
        });
    }


    std::vector<vec2_u16> bp = {};
    int rows = table->rowCount();
    bp.reserve(rows);

    for (int row = 0; row < rows; ++row) {
        auto *edit1 = qobject_cast<QSpinBox*>(table->cellWidget(row, 0));
        auto *edit2 = qobject_cast<QSpinBox*>(table->cellWidget(row, 1));

        if (edit1 && edit2) {
            vec2_u16 point;
            point.il = static_cast<quint16>(edit1->value());
            point.br = static_cast<quint16>(edit2->value()*100);
            bp.push_back(point);
        }
    }
    
    dbus->updateBrakePoints(bp, [this, bp](bool ok, const QString &msg) {
        if(ok){
            qDebug() << "OK >> updateBrakePoints";
            origConfig.brakePoints = bp;
            series->clear();//update chart
            for (vec2_u16 item : origConfig.brakePoints) {
                series->append(item.br/100, item.il);
            }
            chart->update();
        } else {
            qDebug() << "fail >> updateBrakePoints. msg:" << msg;
        }
    });

    startRequestWatcher();
}

void MainView::resetFields(){
    input_loop_delay->setValue(origConfig.loopDelayMs);
    input_change_threshold->setValue(origConfig.changeThreshold);
    input_validation_count->setValue(origConfig.validationCount);
    series->clear();
    table->setRowCount(0);
    for (vec2_u16 item : origConfig.brakePoints) {
        table->removeRow(table->rowCount());
        insertNewPointToTable(item.il, item.br/100);
        series->append(item.br/100, item.il);
    }
    chart->update();
    checkChangesWithConfig();
}

void MainView::startRequestWatcher(){
    if (watcherTimer) {
        return; // уже крутится
    }

    watcherTimer = new QTimer(this);
    connect(watcherTimer, &QTimer::timeout, this, [this]() {

        if (dbus->getRequestСountNow() > 0) {
            qDebug() << "getRequestСountNow > 0";
            if (!rotationTimer || !rotationTimer->isActive()) {
                startSvgUpdateAnimation();
                //this->setAttribute(Qt::WA_TransparentForMouseEvents, false);
                //this->setEnabled(false);
                uiBlocked = true;
                overlay->show();
                overlay->grabKeyboard(); 
            }
        } else {
            qDebug() << "getRequestСountNow == 0";
            if (rotationTimer && rotationTimer->isActive()) {
                stopSvgUpdateAnimation();
                uiBlocked = false;
                overlay->hide();
                overlay->releaseKeyboard();
                // GUI обновляем в главном потоке
                svg_ok->show();
                //svg_update->hide();
                checkChangesWithConfig();
            }
            // все запросы завершились → убиваем watcher
            watcherTimer->stop();
            watcherTimer->deleteLater();
            watcherTimer = nullptr;
        }
    });

    watcherTimer->start(30); 
}

void MainView::startSvgUpdateAnimation(){
    if (!rotationTimer) {
        rotationTimer = new QTimer(this);
        connect(rotationTimer, &QTimer::timeout, this, [this]() {
            rotationAngle += 10;
            if (rotationAngle >= 360) rotationAngle = 0;
    
            QRectF bounds = svg_update_item->boundingRect();
            QTransform transform;
            transform.translate(bounds.width()/2, bounds.height()/2);
            transform.rotate(rotationAngle);
            transform.translate(-bounds.width()/2, -bounds.height()/2);
    
            svg_update_item->setTransform(transform);
        });
    }

    rotationAngle = 0;
    rotationTimer->start(50);
    svg_update_view->show();
}

void MainView::stopSvgUpdateAnimation(){
    if (rotationTimer) {
        rotationTimer->stop();
    }
    svg_update_item->setTransform(QTransform());
    svg_update_view->hide();
}

bool MainView::eventFilter(QObject* obj, QEvent* event)
{
    if (uiBlocked) {
        switch (event->type()) {
            case QEvent::MouseButtonPress:
            case QEvent::MouseButtonRelease:
            case QEvent::MouseButtonDblClick:
            case QEvent::KeyPress:
            case QEvent::KeyRelease:
                return true; // блокируем эти события
            default:
                break;
        }
    }

    // всё остальное пропускаем
    return QWidget::eventFilter(obj, event);
}

void MainView::insertNewPointToTable(){
    insertNewPointToTable(table->rowCount(), 100);
}

void MainView::insertNewPointToTable(quint16 il, quint16 br){
    // временно выключаем сортировку, чтобы операции с индексами были безопасны
    bool wasSorting = table->isSortingEnabled();
    table->setSortingEnabled(false);

    int row = table->rowCount();
    table->insertRow(row);

    // Item для сортировки (колонка 0)
    QTableWidgetItem *item = new QTableWidgetItem;
    // сразу задаём начальное значение для item (для корректной сортировки)
    item->setData(Qt::EditRole, static_cast<int>(il));
    table->setItem(row, 0, item);

    QSpinBox *edit1 = new QSpinBox();
    edit1->setMaximum(MAX_VALUE_12BIT_ADC);
    // блокируем сигналы на время установки начального значения
    {
        QSignalBlocker b(edit1);
        edit1->setValue(il);
    }
    table->setCellWidget(row, 0, edit1);
    connect(edit1, QOverload<int>::of(&QSpinBox::valueChanged), this,
        [this, item](int v) {
            item->setData(Qt::EditRole, v);
            sortListOfPoints();
            checkChangesWithConfig();
        });

    //edit2
    QSpinBox *edit2 = new QSpinBox();
    edit2->setMaximum(100);
    edit2->setValue(br);
    table->setCellWidget(row, 1, edit2);


    // Create a delete button (QToolButton is better for icons than QPushButton)
    QToolButton *btn_del = new QToolButton();
    btn_del->setIcon(QIcon("../assets/delete_24dp.svg"));
    btn_del->setIconSize(QSize(16, 16));
    btn_del->setFixedSize(26, 26);

    // Create a QWidget container for the cell
    QWidget *cellWidget = new QWidget();
    // Use QHBoxLayout to center the button inside the cell
    QHBoxLayout *layout = new QHBoxLayout(cellWidget);
    layout->addWidget(btn_del);
    layout->setContentsMargins(0, 0, 0, 0);
    layout->setAlignment(Qt::AlignCenter);
    cellWidget->setLayout(layout);
    //Place this QWidget (with the button inside) into the table cell (column 2)
    table->setCellWidget(row, 2, cellWidget);
    //Adjust the column width automatically to fit the button
    table->resizeColumnToContents(2);

    // Connect the button click signal to a lambda
    connect(btn_del, &QPushButton::clicked, this, [=](){
        //find the current row index by asking the table for the row of our cellWidget
        int row = table->indexAt(cellWidget->pos()).row();
        if (row >= 0) removePointFromTable(row);// remove the row
    });

    connect(edit2, &QSpinBox::valueChanged, this, &MainView::checkChangesWithConfig);

    // вернуть сортировку в предыдущее состояние и выполнить сортировку один раз
    table->setSortingEnabled(wasSorting);
    if (wasSorting) sortListOfPoints();
    checkChangesWithConfig();
}

void MainView::removePointFromTable(int row){
    table->removeRow(row);//remove point from table
    checkChangesWithConfig();//update ui (aplay/cancle)
}

void MainView::sortListOfPoints(){
    table->sortItems(0, Qt::AscendingOrder);
}
