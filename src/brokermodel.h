#pragma once

#include "broker.h"

#include <QAbstractListModel>
#include <QList>

class BrokerModel : public QAbstractListModel
{
    Q_OBJECT
public:
    BrokerModel(QObject *parent = nullptr);

    void addBroker(const Broker &broker);

    Broker get(int index) const;

    void update(const QModelIndex &index, const Broker &broker);

    QVariant data(const QModelIndex &index, int role = Qt::DisplayRole) const override;

    int rowCount(const QModelIndex &parent = QModelIndex()) const override;

    bool removeRows(int row, int count, const QModelIndex &parent = QModelIndex()) override;

private:
    QList<Broker> brokers;
};
