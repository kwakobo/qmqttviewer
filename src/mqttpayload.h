#pragma once

#include <QByteArray>
#include <QString>

struct MqttPayload
{
    QString topic;
    QByteArray message;

    MqttPayload(const QString &topic, const QByteArray &message) : topic(topic), message(message) {}
};
