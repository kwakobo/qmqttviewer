#pragma once

#include "brokermodel.h"
#include "handler/abstractmessagehandler.h"
#include "messagemodel.h"
#include "subscriptionmodel.h"
#include "topicmodel.h"

#include <QByteArray>
#include <QHash>
#include <QMainWindow>
#include <QMqttClient>
#include <QMqttMessage>
#include <QMqttSubscription>
#include <QMqttTopicName>

QT_BEGIN_NAMESPACE
namespace Ui {
class QMqttViewer;
}
QT_END_NAMESPACE

class QMqttViewer : public QMainWindow
{
    Q_OBJECT
public:
    QMqttViewer(QWidget *parent = nullptr);
    ~QMqttViewer();

    void subscribe(const QString &topic);

private slots:
    void loadSettings();
    void saveSettings();
    void saveUiSettings();

    void handleConnected();
    void handelDisconnected();
    void handleError(QMqttClient::ClientError error);

    void handleConnect();
    void handlePublish();
    void handleSubscribe();
    void handleMessageReceived(const QMqttMessage &message);
    void handleMessageDecoder(int index);
    void handleMessage(const QModelIndex &current, const QModelIndex &previous);
    void clearMessages();
    void scanTopics();
    void handleTopicMessage(const QMqttMessage &message);
    void clearTopics();
    void subscribeTopic();

    void updateMessage(const QModelIndex &index);

    void openAddBrokerDialog();

private:
    Ui::QMqttViewer *ui;

    QMqttClient *client;
    BrokerModel *brokers;
    MessageModel *messages;
    SubscriptionModel *subscriptions;
    TopicModel *topics;

    QMqttSubscription *topicSubscription;

    QHash<QMqttSubscription *, int> counts;
    QList<QWidget *> widgets;

    QList<AbstractMessageHandler *> handlers;
};
