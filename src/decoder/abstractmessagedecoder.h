#pragma once

#include <QByteArray>
#include <QString>

class AbstractMessageDecoder
{
public:
    virtual QString displayName() const = 0;
    virtual QString decodePayload(const QByteArray &message) = 0;
};
