#pragma once

#include <QByteArray>
#include <QString>

class AbstractMessageHandler
{
public:
    enum Features { Encode = 0x1, Decode = 0x2 };

    virtual QString displayName() const = 0;
    virtual int features() const = 0;
    virtual QByteArray encodePayload(const QString &message) const { return {}; };
    virtual QString decodePayload(const QByteArray &message) const { return {}; };
};
