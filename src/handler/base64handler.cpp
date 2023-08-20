#include "base64handler.h"

QString Base64Handler::displayName() const
{
    return QStringLiteral("Base64");
}

QString Base64Handler::decodePayload(const QByteArray &message)
{
    return QByteArray::fromBase64(message);
}
