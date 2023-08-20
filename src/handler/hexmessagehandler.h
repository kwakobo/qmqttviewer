#pragma once

#include "abstractmessagehandler.h"

class HexMessageHandler : public AbstractMessageHandler
{
public:
    QString displayName() const override;
    QString decodePayload(const QByteArray &message) override;
};
