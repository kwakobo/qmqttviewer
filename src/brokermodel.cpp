#include "brokermodel.h"

BrokerModel::BrokerModel(QObject *parent) : QAbstractListModel(parent) {}

void BrokerModel::addBroker(const Broker &broker)
{
    beginInsertRows(QModelIndex(), rowCount(), rowCount());
    brokers.push_back(broker);
    endInsertRows();
}

Broker BrokerModel::get(int index) const
{
    if (index < 0 || index >= rowCount())
        return {};
    return brokers[index];
}

void BrokerModel::update(const QModelIndex &index, const Broker &broker)
{
    if (index.row() < 0 || index.row() >= rowCount())
        return;
    brokers[index.row()] = broker;
    emit dataChanged(index, index);
}

QVariant BrokerModel::data(const QModelIndex &index, int role) const
{
    if (index.row() < 0 || index.row() > rowCount())
        return {};

    switch (role) {
    case Qt::DisplayRole:
        return brokers[index.row()].name;
    default:
        return {};
    }
}

int BrokerModel::rowCount(const QModelIndex &parent) const
{
    if (parent.isValid())
        return 0;
    return brokers.count();
}

bool BrokerModel::removeRows(int row, int count, const QModelIndex &parent)
{
    if (count <= 0 || row < 0 || (row + count) > rowCount(parent))
        return false;
    beginRemoveRows(parent, row, row + count - 1);
    brokers.remove(row, count);
    endRemoveRows();
    return true;
}
