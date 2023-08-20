#include "messagemodel.h"

#include <QMqttTopicName>

MessageModel::MessageModel(QObject *parent) {}

void MessageModel::addMessage(const QMqttMessage &message, int count)
{
    beginInsertRows(QModelIndex(), rowCount(), rowCount());
    messages.push_back(message);
    times.push_back(QDateTime::currentDateTime());
    counts.push_back(count);
    endInsertRows();
}

void MessageModel::removeMessage(const QMqttMessage &message)
{
    for (int i = 0; i < messages.size(); i++) {
        if (messages[i] == message) {
            removeRow(i);
            break;
        }
    }
}

void MessageModel::clear()
{
    removeRows(0, rowCount());
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
    case Count:
        return counts[index.row()];
    case Qos:
        return messages[index.row()].qos();
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

bool MessageModel::removeRows(int row, int count, const QModelIndex &parent)
{
    if (count <= 0 || row < 0 || (row + count) > rowCount(parent))
        return false;
    beginRemoveRows(parent, row, row + count - 1);
    messages.remove(row, count);
    times.remove(row, count);
    counts.remove(row, count);
    endRemoveRows();
    return true;
}
