#ifndef DBUSCLIENT_H
#define DBUSCLIENT_H

#include <iostream>
#include <vector>

#include <QObject>
#include <QDBusInterface>
#include <QDBusReply>
#include <QDBusConnection>
#include <QDBusMetaType>
#include <QDBusArgument>
#include <QMetaType>
#include <QList>
#include <QVariant>

#include "../h/vec2_u16.h"

Q_DECLARE_METATYPE(vec2_u16)


typedef QList<vec2_u16> Vec2List;
Q_DECLARE_METATYPE(Vec2List)


class DbusClient : public QObject {
    Q_OBJECT
    private:
        QDBusInterface interface;
    public:
        explicit DbusClient(QObject *parent = nullptr);
        void requestIlluminance();
        void requestLoopDelayMs();
        void requestTHR();
        void requestValidationCount();
        void requestPoints();

    signals:
        void illuminanceReceived(short value);
        void loopDelayMsReceived(short value);
        void THRReceived(short value);
        void validationCountReceived(short value);
        void pointsReceived(Vec2List list);

};


#endif