#include "hexmessagedecoder.h"

QString HexMessageDecoder::displayName() const
{
    return QStringLiteral("Hex Decoder");
}

QString HexMessageDecoder::decodePayload(const QByteArray &message)
{
    return message.toHex(' ');
}
