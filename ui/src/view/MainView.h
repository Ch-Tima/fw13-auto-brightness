#ifndef MAINVIEW_H
#define MAINVIEW_H

#include <iostream>

#include <QWidget>
#include <QVBoxLayout>
#include <QGridLayout>
#include <QFormLayout>
#include <QLineEdit>
#include <QSpinBox>
#include <QPalette>

#include <QLineSeries>
#include <QChart>
#include <QChartView>

#include<QTableWidget>
#include<QLabel>

#include<QHBoxLayout>
#include<QPushButton>

#include <QDBusArgument>
#include <QMetaType>
#include <QDBusMetaType>
#include <QList>
#include <QTimer>
#include <QValueAxis>
#include <QSvgWidget>
#include <QGraphicsSvgItem>
#include <QIcon>
#include <QRect>
#include <QRegion>

#include <QtConcurrent>

#include "dbus/DbusClient.h"
#include "../h/Config.h"
#include "../h/vec2_u16.h"

class MainView : public QWidget {
    Q_OBJECT
public:
    explicit MainView(QWidget *parent = nullptr);
protected:
    bool eventFilter(QObject* obj, QEvent* event) override;  
private:
    QVBoxLayout *layout = nullptr;
    QGridLayout *main_layout = nullptr;
    QFormLayout *form = nullptr;
    QSpinBox *input_change_threshold = nullptr;
    QSpinBox *input_validation_count = nullptr;
    QSpinBox *input_loop_delay = nullptr;
    QWidget *overlay = nullptr;
    //Chart
    QLineSeries *series = nullptr;
    QChart *chart = nullptr;
    QChartView *chartView = nullptr;
    //Table
    QTableWidget *table = nullptr;
    //iluminance now
    QLabel *current_il_value = nullptr;
    //Buttons
    QHBoxLayout *bottom_btn_layout = nullptr;
    QPushButton *btn_cancel = nullptr;
    QPushButton *btn_applay = nullptr;
    QPushButton *btn_add_points = nullptr;

    //QSvgWidget *svg_update = nullptr;
    QSvgWidget *svg_ok = nullptr;

    QGraphicsView* svg_update_view = nullptr;
    QGraphicsSvgItem* svg_update_item = nullptr;

    //DBus
    DbusClient *dbus;
    Config origConfig;

    //Timers
    QTimer* watcherTimer = nullptr;
    QTimer* rotationTimer = nullptr;

    int rotationAngle = 0;
    bool uiBlocked = false;

    static const short MAX_VALUE_12BIT_ADC = 4095;

    int init();
    int updateChart();
    int convertToValidNumber(const QString &text, int min, int max);
    void checkChangesWithConfig();
    void applayConfigToDemon();
    /*
        Method for resetting all fields to their initial values ​​from origConfig:Config
    */
    void resetFields();
    void startRequestWatcher();

    void insertNewPointToTable(quint16 il, quint16 br);
    void insertNewPointToTable();

    void startSvgUpdateAnimation();
    void stopSvgUpdateAnimation();

    void sortListOfPoints();
};

#endif