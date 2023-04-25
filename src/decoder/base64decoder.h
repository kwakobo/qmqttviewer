#pragma once

#include "abstractmessagedecoder.h"

class Base64Decoder : public AbstractMessageDecoder
{
    QString displayName() const override;
    QString decodePayload(const QByteArray &message) override;
};