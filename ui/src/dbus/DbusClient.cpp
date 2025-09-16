#include "DbusClient.h"

QDBusArgument &operator<<(QDBusArgument &argument, const vec2_u16 &point)
{
    argument.beginStructure();
    argument << point.il << point.br;
    argument.endStructure();
    return argument;
}

const QDBusArgument &operator>>(const QDBusArgument &argument, vec2_u16 &point)
{
    argument.beginStructure();
    argument >> point.il >> point.br;
    argument.endStructure();
    return argument;
}

DbusClient::DbusClient(QObject *parent) : 
    QObject(parent),
    interface(
        "com.ct.AutoBrightness",    // service name (bus name, тот что в --dest)
        "/com/ct/AutoBrightness",   // object path (тот что в dbus-send после dest)
        "com.ct.AutoBrightness",    // interface (тот что в dbus-send после object path)
        QDBusConnection::sessionBus()
    ){

    if(!interface.isValid()){
        std::cout << "ERR: I can not connect to com.ct.AutoBrightness" << std::endl;
    }

    qDBusRegisterMetaType<vec2_u16>();
    qDBusRegisterMetaType<Vec2List>();

    //TEST
    // Vec2List v 
    // {
    //     { 0,    500   }, 
    //     { 20,   3000  }, 
    //     { 80,   4000  }, 
    //     { 100,  5000  }, 
    //     { 500,  5500  },   
    //     { 300,  6000  },   
    //     { 200,  7000  },
    //     { 1400, 8500  },
    //     { 3355, 10000 },
    // };
    // QList<QVariant> args;
    // args << QVariant::fromValue(v);

    // QDBusReply<void> reply = interface.callWithArgumentList(QDBus::Block, "SetVectorBrakePoints", args);
    // if(reply.isValid()){
    //     std::cout << "OK" << std::endl;
    // }else{
    //     std::cout << "NOK" << std::endl;
    // }

    //END_TEST
}


void DbusClient::requestIlluminance(){

    QDBusReply<uint16_t> reply = interface.call("GetIlluminance");
    if (reply.isValid()){
        std::cout << reply.value() << std::endl;
        emit illuminanceReceived(reply.value());
       
    }else{
        emit illuminanceReceived(-1);
    }
}

void DbusClient::requestLoopDelayMs(){
    QDBusReply<uint16_t> replyLoopDelayMs = interface.call("GetLoopDelayMs");
    if(replyLoopDelayMs.isValid()){
        emit loopDelayMsReceived(replyLoopDelayMs.value());
    }else{
        emit loopDelayMsReceived(-1);
    }
}

void DbusClient::requestTHR(){
    QDBusReply<uint16_t> replyTHR = interface.call("GetChangeThreshold");
    if(replyTHR.isValid()){
        emit THRReceived(replyTHR.value());
    }else{
        emit THRReceived(-1);
    }
}

void DbusClient::requestValidationCount(){
    QDBusReply<uint8_t> replyValidationCount = interface.call("GetValidationCount");
    if(replyValidationCount.isValid()){
        emit validationCountReceived(replyValidationCount.value());
    }else{
        emit validationCountReceived(-1);
    }
}

void DbusClient::requestPoints(){
    QDBusReply<Vec2List> v2r = interface.call("GetVectorBrakePoints");
    if(v2r.isValid()){
        std::cout << "OK!" << std::endl;
        emit pointsReceived(v2r.value());
    }else{
        std::cout << "NO SIGNAL GetVectorBrakePoints failed!" << std::endl;
        qWarning() << "DbusClient: GetVectorBrakePoints failed" << v2r.error().message();
        emit pointsReceived({});
    }
}

//++++++++++++++++/SET/++++++++++++++++//

void DbusClient::setLoopDelay(quint16 value, Callback cb){
    requestСountNow++;
    QVariant arg = QVariant::fromValue<quint16>(value);
    QDBusPendingCall asyncCall = interface.asyncCall("SetLoopDelayMs", arg);
    QDBusPendingCallWatcher *watcher = new QDBusPendingCallWatcher(asyncCall, this);

    connect(watcher, &QDBusPendingCallWatcher::finished, this,
        [watcher, cb, this](QDBusPendingCallWatcher *w) {
            QDBusPendingReply<> reply = *w;
            w->deleteLater();
            if (reply.isError()) {
                cb(false, reply.error().message());
            } else {
                cb(true, "ok");
            }
            requestСountNow--;
        }
    );
}

void DbusClient::setChangeThreshold(quint16 value, Callback cb){
    requestСountNow++;
    QVariant arg = QVariant::fromValue<quint16>(value);
    QDBusPendingCall replyAsync = interface.asyncCall("SetChangeThreshold", arg);
    QDBusPendingCallWatcher *watcher = new QDBusPendingCallWatcher(replyAsync, this);

    connect(watcher, &QDBusPendingCallWatcher::finished, this, 
    [watcher, cb, this](QDBusPendingCallWatcher *w){
        QDBusPendingReply<> reply = *w;
        w->deleteLater();
        if (reply.isError()) {
            cb(false, reply.error().message());
        } else {
            cb(true, "ok");
        }
        requestСountNow--;
    });
}

void DbusClient::setValidationCount(quint8 value, Callback cb){
    requestСountNow++;
    QVariant arg = QVariant::fromValue<quint8>(value);
    QDBusPendingCall asyncCall = interface.asyncCall("SetValidationCount", arg);
    QDBusPendingCallWatcher *watcher = new QDBusPendingCallWatcher(asyncCall, this);
    
    connect(watcher, &QDBusPendingCallWatcher::finished, this,
        [watcher, cb, this](QDBusPendingCallWatcher *w) {
            QDBusPendingReply<> reply = *w;
            w->deleteLater();
            if (reply.isError()) {
                cb(false, reply.error().message());
            } else {
                cb(true, "ok");
            }
            requestСountNow--;
        }
    );
}

uint16_t DbusClient::getRequestСountNow(){
    return requestСountNow.load();
}

