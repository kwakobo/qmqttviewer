#include "base64handler.h"

QString Base64Handler::displayName() const
{
    return QStringLiteral("Base64");
}

int Base64Handler::features() const
{
    return Encode | Decode;
}

QByteArray Base64Handler::encodePayload(const QString &message) const
{
    return message.toLatin1().toBase64();
}

QString Base64Handler::decodePayload(const QByteArray &message) const
{
    return QByteArray::fromBase64(message);
}
