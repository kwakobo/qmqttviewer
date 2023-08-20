#include "hexmessagehandler.h"

QString HexMessageHandler::displayName() const
{
    return QStringLiteral("Hex");
}

int HexMessageHandler::features() const
{
    return Decode;
}

QString HexMessageHandler::decodePayload(const QByteArray &message) const
{
    return message.toHex(' ');
}
