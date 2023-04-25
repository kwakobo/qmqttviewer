#include "messagemodel.h"

#include <QMqttTopicName>

MessageModel::MessageModel(QObject *parent) {}

void MessageModel::addMessage(const QMqttMessage &message)
{
    beginInsertRows(QModelIndex(), rowCount(), rowCount());
    messages.push_back(message);
    times.push_back(QDateTime::currentDateTime());
    endInsertRows();
}

QVariant MessageModel::data(const QModelIndex &index, int role) const
{
    if (index.row() < 0 || index.row() > rowCount())
        return {};

    switch (role) {
    case Qt::DisplayRole:
    case Topic:
        return messages[index.row()].topic().name();
    case Message:
        return messages[index.row()].payload();
    case DateTime:
        return times[index.row()];
    default:
        return {};
    }
}

int MessageModel::rowCount(const QModelIndex &parent) const
{
    if (parent.isValid())
        return 0;
    return messages.count();
}
