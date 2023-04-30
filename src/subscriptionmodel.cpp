#include "subscriptionmodel.h"

SubscriptionModel::SubscriptionModel(QObject *parent) : QAbstractListModel(parent) {}

void SubscriptionModel::addSubscription(QMqttSubscription *subscription)
{
    beginInsertRows(QModelIndex(), rowCount(), rowCount());
    subscriptions.push_back(subscription);
    endInsertRows();
}

void SubscriptionModel::toggleMute(const QModelIndex &index)
{
    if (index.row() < 0 || index.row() > rowCount())
        return;
    auto subscription = subscriptions[index.row()];
    if (muted.contains(subscription))
        muted.remove(subscription);
    else
        muted.insert(subscription);
}

bool SubscriptionModel::isMuted(QMqttSubscription *subscription) const
{
    return muted.contains(subscription);
}

bool SubscriptionModel::isMuted(const QModelIndex &index) const
{
    if (index.row() < 0 || index.row() > rowCount())
        return false;
    auto subscription = subscriptions[index.row()];
    return isMuted(subscription);
}

void SubscriptionModel::clear()
{
    removeRows(0, rowCount());
}

bool SubscriptionModel::contains(const QString &topic) const
{
    for (int i = 0; i < subscriptions.count(); i++) {
        if (subscriptions[i]->topic().filter() == topic)
            return true;
    }
    return false;
}

QVariant SubscriptionModel::data(const QModelIndex &index, int role) const
{
    if (index.row() < 0 || index.row() > rowCount())
        return {};

    switch (role) {
    case Qt::DisplayRole:
        return subscriptions[index.row()]->topic().filter();
    default:
        return {};
    }
}

int SubscriptionModel::rowCount(const QModelIndex &parent) const
{
    if (parent.isValid())
        return 0;
    return subscriptions.count();
}

bool SubscriptionModel::removeRows(int row, int count, const QModelIndex &parent)
{
    if (count <= 0 || row < 0 || (row + count) > rowCount(parent))
        return false;
    beginRemoveRows(parent, row, row + count - 1);
    auto subscription = subscriptions.at(row);
    subscription->unsubscribe();
    subscriptions.remove(row, count);
    endRemoveRows();
    return true;
}
