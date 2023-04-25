#pragma once

#include "brokermodel.h"
#include "decoder/abstractmessagedecoder.h"
#include "messagemodel.h"
#include "subscriptionmodel.h"

#include <QByteArray>
#include <QMainWindow>
#include <QMqttClient>
#include <QMqttMessage>
#include <QMqttTopicName>
#include <QStringListModel>

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

private slots:
    void loadSettings();
    void saveSettings();

    void handleConnected();
    void handelDisconnected();

    void handleConnect();
    void handlePublish();
    void handleSubscribe();
    void handleMessageReceived(const QMqttMessage &message);
    void handleMessageDecoder(int index);
    void handleMessage(const QModelIndex &current, const QModelIndex &previous);

    void updateMessage(const QModelIndex &index);

    void openAddBrokerDialog();

private:
    Ui::QMqttViewer *ui;

    QMqttClient *client;
    BrokerModel *brokers;
    MessageModel *messages;
    SubscriptionModel *subscriptions;

    QList<QWidget *> widgets;

    QList<AbstractMessageDecoder *> decoders;
};