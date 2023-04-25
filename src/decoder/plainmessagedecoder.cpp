#include "plainmessagedecoder.h"

QString PlainMessageDecoder::displayName() const
{
    return QStringLiteral("Plain Text Decoder");
}

QString PlainMessageDecoder::decodePayload(const QByteArray &message)
{
    return message;
}
