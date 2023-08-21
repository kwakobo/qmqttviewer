#include "topicmodel.h"

TopicModel::TopicModel(QObject *parent) : QStringListModel(parent) {}

void TopicModel::addTopic(const QString &topic)
{
    if (topics.contains(topic))
        return;
    if (!insertRow(rowCount()))
        return;
    setData(index(rowCount() - 1, 0), topic);
    topics.insert(topic);
}

void TopicModel::clear()
{
    removeRows(0, rowCount());
    topics.clear();
}
