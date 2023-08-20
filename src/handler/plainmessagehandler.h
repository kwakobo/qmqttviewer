#pragma once

#include "abstractmessagehandler.h"

class PlainMessageHandler : public AbstractMessageHandler
{
public:
    QString displayName() const override;
    int features() const override;
    QByteArray encodePayload(const QString &message) const override;
    QString decodePayload(const QByteArray &message) const override;
};
