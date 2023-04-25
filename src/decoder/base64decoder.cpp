#include "base64decoder.h"

QString Base64Decoder::displayName() const
{
    return QStringLiteral("Base64 Decoder");
}

QString Base64Decoder::decodePayload(const QByteArray &message)
{
    return QByteArray::fromBase64(message);
}
