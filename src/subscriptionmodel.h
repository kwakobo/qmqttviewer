#pragma once

#include <QAbstractListModel>
#include <QList>
#include <QMqttSubscription>

class SubscriptionModel : public QAbstractListModel
{
public:
    SubscriptionModel(QObject *parent = nullptr);

    void addSubscription(QMqttSubscription *subscription);

    void clear();

    bool contains(const QString &topic) const;

    QVariant data(const QModelIndex &index, int role = Qt::DisplayRole) const override;

    int rowCount(const QModelIndex &parent = QModelIndex()) const override;

    bool removeRows(int row, int count, const QModelIndex &parent = QModelIndex()) override;

private:
    QList<QMqttSubscription *> subscriptions;
};
