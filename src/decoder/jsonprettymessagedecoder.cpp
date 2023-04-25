#include "jsonprettymessagedecoder.h"

#include <QJsonDocument>

QString JsonPrettyMessageDecoder::displayName() const
{
    return QStringLiteral("JSON Pretty Fromat Decoder");
}

QString JsonPrettyMessageDecoder::decodePayload(const QByteArray &message)
{
    QJsonParseError error;
    auto doc = QJsonDocument::fromJson(message, &error);
    if (error.error != QJsonParseError::NoError)
        return error.errorString();
    return doc.toJson();
}
