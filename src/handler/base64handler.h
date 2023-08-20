#pragma once

#include "abstractmessagehandler.h"

class Base64Handler : public AbstractMessageHandler
{
public:
    QString displayName() const override;
    QString decodePayload(const QByteArray &message) const override;
};
