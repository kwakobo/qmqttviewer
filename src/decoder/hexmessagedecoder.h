#pragma once

#include "abstractmessagedecoder.h"

class HexMessageDecoder : public AbstractMessageDecoder
{
    QString displayName() const override;
    QString decodePayload(const QByteArray &message) override;
};
