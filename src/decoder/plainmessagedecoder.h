#pragma once

#include "abstractmessagedecoder.h"

class PlainMessageDecoder : public AbstractMessageDecoder
{
public:
    QString displayName() const override;
    QString decodePayload(const QByteArray &message) override;
};
