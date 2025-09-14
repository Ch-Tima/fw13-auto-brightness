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
        void setChangeThreshold(quint16 value);

        uint16_t getRequestСountNow();

    signals:
        void illuminanceReceived(short value);
        void loopDelayMsReceived(short value);
        void THRReceived(short value);
        void validationCountReceived(short value);
        void pointsReceived(Vec2List list);

};


#endif