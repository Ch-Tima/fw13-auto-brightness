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

#ifndef DBUSCLIENT_H
#define DBUSCLIENT_H

#include <iostream>
#include <vector>
#include <functional>
#include <atomic>

#include <QObject>
#include <QDBusInterface>
#include <QDBusReply>
#include <QDBusConnection>
#include <QDBusMetaType>
#include <QDBusArgument>
#include <QMetaType>
#include <QList>
#include <QVariant>
#include <QThread>

#include "../h/vec2_u16.h"
#include "../h/Config.h"

Q_DECLARE_METATYPE(vec2_u16)


typedef QList<vec2_u16> Vec2List;
Q_DECLARE_METATYPE(Vec2List)


class DbusClient : public QObject {
    Q_OBJECT
    private:
        QDBusInterface interface;
        std::atomic<uint16_t> requestСountNow{0};
    public:
        explicit DbusClient(QObject *parent = nullptr);
        
        using Callback = std::function<void(bool success, const QString &msg)>;

        void requestIlluminance();
        void requestLoopDelayMs();
        void requestTHR();
        void requestValidationCount();
        void requestPoints();

        void setLoopDelay(quint16 value, Callback cb);
        void setValidationCount(quint8 value, Callback cb);
        void setChangeThreshold(quint16 value, Callback cb);
        void updateBrakePoints(std::vector<vec2_u16> &brakePoints, Callback cb);

        uint16_t getRequestСountNow();

    signals:
        void illuminanceReceived(short value);
        void loopDelayMsReceived(short value);
        void THRReceived(short value);
        void validationCountReceived(short value);
        void pointsReceived(Vec2List list);

};


#endif