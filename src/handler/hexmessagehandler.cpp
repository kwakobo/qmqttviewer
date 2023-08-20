#include "hexmessagehandler.h"

QString HexMessageHandler::displayName() const
{
    return QStringLiteral("Hex");
}

QString HexMessageHandler::decodePayload(const QByteArray &message) const
{
    return message.toHex(' ');
}
