#pragma once

#include <QAbstractListModel>
#include <QDateTime>
#include <QList>
#include <QMqttMessage>

class MessageModel : public QAbstractListModel
{
    Q_OBJECT
public:
    enum Type {
        Topic = Qt::UserRole + 1,
        Message,
        DateTime,
    };

    MessageModel(QObject *parent = nullptr);

    void addMessage(const QMqttMessage &message);

    QVariant data(const QModelIndex &index, int role = Qt::DisplayRole) const override;

    int rowCount(const QModelIndex &parent = QModelIndex()) const override;

private:
    QList<QMqttMessage> messages;
    QList<QDateTime> times;
};
