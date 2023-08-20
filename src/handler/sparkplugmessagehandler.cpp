#include "sparkplugmessagehandler.h"
#include "sparkplug_b.pb.h"

#include <QDebug>
#include <QObject>

#include <string>

#include <google/protobuf/util/json_util.h>

namespace sparkplug = org::eclipse::tahu::protobuf;
namespace util = google::protobuf::util;

QString SparkplugMessageHandler::displayName() const
{
    return QStringLiteral("Sparkplug");
}

int SparkplugMessageHandler::features() const
{
    return Encode | Decode;
}

QByteArray SparkplugMessageHandler::encodePayload(const QString &message) const
{
    sparkplug::Payload payload;
    auto result = util::JsonStringToMessage(message.toStdString(), &payload);
    if (!result.ok()) {
        qWarning() << result.ToString();
        return {};
    }

    return QByteArray::fromStdString(payload.SerializeAsString());
}

QString SparkplugMessageHandler::decodePayload(const QByteArray &message) const
{
    sparkplug::Payload payload;
    if (!payload.ParseFromString(message.toStdString()))
        return QObject::tr("Failed to parse payload.");

    util::JsonPrintOptions opt;
    opt.add_whitespace = true;
    std::string output;
    auto status = util::MessageToJsonString(payload, &output, opt);
    if (!status.ok())
        return QObject::tr("Failed to serialize payload");
    return QString::fromStdString(output);
}
