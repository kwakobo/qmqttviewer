#include "plainmessagehandler.h"

QString PlainMessageHandler::displayName() const
{
    return QStringLiteral("Plain Text");
}

QString PlainMessageHandler::decodePayload(const QByteArray &message) const
{
    return message;
}
