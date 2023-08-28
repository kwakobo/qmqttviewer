#include "jsonprettymessagehandler.h"

#include <QJsonDocument>

QString JsonPrettyMessageHandler::displayName() const
{
    return QStringLiteral("JSON Pretty Format");
}

int JsonPrettyMessageHandler::features() const
{
    return Decode;
}

QString JsonPrettyMessageHandler::decodePayload(const QByteArray &message) const
{
    QJsonParseError error;
    auto doc = QJsonDocument::fromJson(message, &error);
    if (error.error != QJsonParseError::NoError)
        return error.errorString();
    return doc.toJson();
}
