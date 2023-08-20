#include "plainmessagehandler.h"

QString PlainMessageHandler::displayName() const
{
    return QStringLiteral("Plain Text");
}

int PlainMessageHandler::features() const
{
    return Encode | Decode;
}

QByteArray PlainMessageHandler::encodePayload(const QString &message) const
{
    return message.toLatin1();
}

QString PlainMessageHandler::decodePayload(const QByteArray &message) const
{
    return message;
}
