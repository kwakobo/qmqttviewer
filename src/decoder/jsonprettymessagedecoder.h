#pragma once

#include "abstractmessagedecoder.h"

class JsonPrettyMessageDecoder : public AbstractMessageDecoder
{
public:
    QString displayName() const override;
    QString decodePayload(const QByteArray &message) override;
};
