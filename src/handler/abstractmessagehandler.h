#pragma once

#include <QByteArray>
#include <QString>

class AbstractMessageHandler
{
public:
    virtual QString displayName() const = 0;
    virtual QString decodePayload(const QByteArray &message) const = 0;
};
