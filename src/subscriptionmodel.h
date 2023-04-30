#pragma once

#include <QAbstractListModel>
#include <QList>
#include <QMqttSubscription>
#include <QSet>

class SubscriptionModel : public QAbstractListModel
{
    Q_OBJECT
public:
    SubscriptionModel(QObject *parent = nullptr);

    void addSubscription(QMqttSubscription *subscription);

    void toggleMute(const QModelIndex &index);

    bool isMuted(QMqttSubscription *subscription) const;

    bool isMuted(const QModelIndex &index) const;

    void clear();

    bool contains(const QString &topic) const;

    QVariant data(const QModelIndex &index, int role = Qt::DisplayRole) const override;

    int rowCount(const QModelIndex &parent = QModelIndex()) const override;

    bool removeRows(int row, int count, const QModelIndex &parent = QModelIndex()) override;

private:
    QSet<QMqttSubscription *> muted;
    QList<QMqttSubscription *> subscriptions;
};
