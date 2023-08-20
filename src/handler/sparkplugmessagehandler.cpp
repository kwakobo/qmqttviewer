#include "sparkplugmessagehandler.h"
#include "sparkplug_b.pb.h"

#include <QObject>

#include <string>

#include <google/protobuf/util/json_util.h>

namespace sparkplug = org::eclipse::tahu::protobuf;
namespace util = google::protobuf::util;

QString SparkplugMessageHandler::displayName() const
{
    return QStringLiteral("Sparkplug");
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
