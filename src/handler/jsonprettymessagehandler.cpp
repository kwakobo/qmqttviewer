#include "jsonprettymessagehandler.h"

#include <QJsonDocument>

QString JsonPrettyMessageHandler::displayName() const
{
    return QStringLiteral("JSON Pretty Fromat");
}

QString JsonPrettyMessageHandler::decodePayload(const QByteArray &message) const
{
    QJsonParseError error;
    auto doc = QJsonDocument::fromJson(message, &error);
    if (error.error != QJsonParseError::NoError)
        return error.errorString();
    return doc.toJson();
}
