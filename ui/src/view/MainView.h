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

#include "dbus/DbusClient.h"
#include "../h/Config.h"
#include "../h/vec2_u16.h"

class MainView : public QWidget {
    Q_OBJECT
public:
    explicit MainView(QWidget *parent = nullptr);
        
private:
    QVBoxLayout *layout = nullptr;
    QGridLayout *main_layout = nullptr;
    QFormLayout *form = nullptr;
    QSpinBox *input_limit = nullptr;
    QSpinBox *input_check = nullptr;
    QSpinBox *input_time = nullptr;
    //Chart
    QLineSeries *series = nullptr;
    QChart *chart = nullptr;
    QChartView *chartView = nullptr;
    //Table
    QTableWidget *table = nullptr;
    //
    QLabel *current_il_value = nullptr;
    //Buttons
    QHBoxLayout *bottom_btn_layout = nullptr;
    QPushButton *btn_cancel = nullptr;
    QPushButton *btn_applay = nullptr;

    //DBus
    DbusClient *dbus;
    Config conf;
    Config confNew;

    int init();
    int updateChart();
    int convertToValidNumber(const QString &text, int min, int max);
    
};

#endif