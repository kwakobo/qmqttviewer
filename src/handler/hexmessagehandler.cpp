#include "hexmessagehandler.h"

QString HexMessageHandler::displayName() const
{
    return QStringLiteral("Hex");
}

int HexMessageHandler::features() const
{
    return Encode | Decode;
}

QByteArray HexMessageHandler::encodePayload(const QString &message) const
{
    return QByteArray::fromHex(message.toLatin1());
}

QString HexMessageHandler::decodePayload(const QByteArray &message) const
{
    return message.toHex(' ');
}
