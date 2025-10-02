/*
 * fw13-auto-brightness
 * Part of fw13-auto-brightness project (AutoBrightnessUI / AutoBrightnessIluminance)
 * Copyright (C) 2025  <Ch-Tima>
 *
 * This program is free software: you can redistribute it and/or modify 
 * it under the terms of the GNU Affero General Public License as published by the Free Software Foundation, either version 3 of the License, or (at your option) any later version.
 *
 * This program is distributed in the hope that it will be useful, but WITHOUT ANY WARRANTY; without even the implied warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.
 * See the GNU Affero General Public License for more details.
 *
 * You should have received a copy of the GNU Affero General Public License
 * along with this program.  If not, see <https://www.gnu.org/licenses/>.
*/

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

#include <QHBoxLayout>
#include <QPushButton>
#include <QToolButton>

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

#include <QHeaderView>

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

    void insertNewPointToTable();
    void insertNewPointToTable(quint16 il, quint16 br);
    void removePointFromTable(int row);

    void startSvgUpdateAnimation();
    void stopSvgUpdateAnimation();

    void sortListOfPoints();
};

#endif