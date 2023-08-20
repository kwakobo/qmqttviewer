#pragma once

#include "abstractmessagehandler.h"

class JsonPrettyMessageHandler : public AbstractMessageHandler
{
public:
    QString displayName() const override;
    QString decodePayload(const QByteArray &message) const override;
};
