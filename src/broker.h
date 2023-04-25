#pragma once

#include <QMqttClient>
#include <QObject>
#include <QSsl>
#include <QString>
#include <QUuid>

struct Broker
{
    QString name;
    QString address;
    int port;
    QString clientId;

    int keepAlive;
    bool cleanSession;
    bool autoReconnect;
    QMqttClient::ProtocolVersion mqttVersion;

    QString username;
    QString password;

    bool useSsl;
    QSsl::SslProtocol sslProtocol;

    static Broker defaultBroker()
    {
        Broker broker;

        broker.name = QObject::tr("New Profile");
        broker.address = QStringLiteral("127.0.0.1");
        broker.port = 1883;
        broker.clientId = QUuid::createUuid().toString(QUuid::WithoutBraces);

        broker.keepAlive = 60;
        broker.cleanSession = true;
        broker.autoReconnect = false;
        broker.mqttVersion = QMqttClient::MQTT_3_1_1;

        broker.useSsl = false;
        broker.sslProtocol = QSsl::TlsV1_2;

        return broker;
    }
};
