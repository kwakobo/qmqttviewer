#pragma once

#include <QSet>
#include <QStringListModel>

class TopicModel : public QStringListModel
{
    Q_OBJECT
public:
    TopicModel(QObject *parent = nullptr);

    void addTopic(const QString &topic);

    void clear();

private:
    QSet<QString> topics;
};
