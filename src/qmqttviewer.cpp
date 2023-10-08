#include "qmqttviewer.h"
#include "brokerdialog.h"
#include "messageitemdelegate.h"
#include "subscriptionitemdelegate.h"
#include "ui_qmqttviewer.h"

#include "handler/base64handler.h"
#include "handler/hexmessagehandler.h"
#include "handler/jsonprettymessagehandler.h"
#include "handler/plainmessagehandler.h"
#include "handler/sparkplugmessagehandler.h"

#include <QButtonGroup>
#include <QSettings>

#include <algorithm>

static QPlainTextEdit *edit = nullptr;

static void log(QtMsgType type, const QMessageLogContext &context, const QString &msg);

QMqttViewer::QMqttViewer(QWidget *parent) :
    QMainWindow(parent), ui(new Ui::QMqttViewer), client(new QMqttClient(this)),
    brokers(new BrokerModel(this)), messages(new MessageModel(this)),
    subscriptions(new SubscriptionModel(this)), topics(new TopicModel), topicSubscription(nullptr),
    handlers({
        new PlainMessageHandler,
        new JsonPrettyMessageHandler,
        new Base64Handler,
        new HexMessageHandler,
        new SparkplugMessageHandler,
    })
{
    ui->setupUi(this);

    for (int i = 0; i < handlers.size(); i++) {
        if (handlers[i]->features() & AbstractMessageHandler::Encode)
            ui->encoder->insertItem(i, handlers[i]->displayName() + QStringLiteral(" Encoder"), i);
        if (handlers[i]->features() & AbstractMessageHandler::Decode)
            ui->decoder->insertItem(i, handlers[i]->displayName() + QStringLiteral(" Decoder"), i);
    }

    ui->broker->setModel(brokers);
    ui->messages->setModel(messages);
    ui->subscriptions->setModel(subscriptions);
    ui->topics->setModel(topics);

    auto publishQos = new QButtonGroup(this);
    publishQos->addButton(ui->publishQos0);
    publishQos->addButton(ui->publishQos1);
    publishQos->addButton(ui->publishQos2);

    auto subscribeQos = new QButtonGroup(this);
    subscribeQos->addButton(ui->subscriptionQos0);
    subscribeQos->addButton(ui->subscriptionQos1);
    subscribeQos->addButton(ui->subscriptionQos2);

    ui->subscriptions->setItemDelegate(new SubscriptionItemDelegate(this));
    ui->messages->setItemDelegate(new MessageItemDelegate(this));

    auto additionalMenu = new QMenu;
    auto clear = additionalMenu->addAction(tr("Clear messages"));
    ui->additional->setMenu(additionalMenu);

    auto topicAdditionalMenu = new QMenu;
    auto clearTopics = topicAdditionalMenu->addAction(tr("Clear topics"));
    ui->topicAdditional->setMenu(topicAdditionalMenu);

    auto addTopic = ui->topics->addAction(tr("Subscribe"));

    connect(ui->exit, &QAction::triggered, qApp, &QApplication::exit);
    connect(ui->editConnections, &QAction::triggered, this, &QMqttViewer::openAddBrokerDialog);
    connect(ui->about, &QAction::triggered, qApp, &QApplication::aboutQt);
    connect(ui->connect, &QPushButton::clicked, this, &QMqttViewer::handleConnect);
    connect(ui->publish, &QPushButton::clicked, this, &QMqttViewer::handlePublish);
    connect(ui->subscribe, &QPushButton::clicked, this, &QMqttViewer::handleSubscribe);
    connect(ui->decoder, &QComboBox::activated, this, &QMqttViewer::handleMessageDecoder);
    connect(ui->additional, &QToolButton::clicked, ui->additional, &QToolButton::showMenu);
    connect(clear, &QAction::triggered, this, &QMqttViewer::clearMessages);
    connect(ui->scan, &QPushButton::clicked, this, &QMqttViewer::scanTopics);
    connect(clearTopics, &QAction::triggered, this, &QMqttViewer::clearTopics);
    connect(addTopic, &QAction::triggered, this, &QMqttViewer::subscribeTopic);

    connect(ui->messages->selectionModel(),
            &QItemSelectionModel::currentChanged,
            this,
            &QMqttViewer::handleMessage);
    connect(messages, &MessageModel::rowsInserted, ui->messages, [this]() {
        if (!ui->autoscroll->isChecked())
            return;
        auto index = messages->index(messages->rowCount() - 1);
        ui->messages->scrollToBottom();
        ui->messages->selectionModel()->setCurrentIndex(index, QItemSelectionModel::ClearAndSelect);
    });

    connect(brokers, &BrokerModel::rowsInserted, this, [this]() { ui->connect->setEnabled(true); });
    connect(brokers, &BrokerModel::rowsRemoved, this, [this]() {
        ui->connect->setEnabled(brokers->rowCount() != 0);
    });

    connect(client, &QMqttClient::connected, this, &QMqttViewer::handleConnected);
    connect(client, &QMqttClient::disconnected, this, &QMqttViewer::handelDisconnected);
    connect(client, &QMqttClient::errorChanged, this, &QMqttViewer::handleError);

    widgets = {ui->publishTab, ui->subscribeTab};

    edit = ui->logTab;
    qInstallMessageHandler(log);

    loadSettings();
}

QMqttViewer::~QMqttViewer()
{
    saveUiSettings();

    delete client;
    delete ui;
    qDeleteAll(handlers);
}

static int qos(const QList<QCheckBox *> &checkboxes)
{
    for (int i = 0; i < checkboxes.size(); i++) {
        if (checkboxes[i]->isChecked())
            return i;
    }
    return 0;
}

void QMqttViewer::subscribe(const QString &topic)
{
    if (topic.isEmpty() || subscriptions->contains(topic))
        return;
    auto subscription = client->subscribe(topic,
                                          qos({ui->subscriptionQos0,
                                               ui->subscriptionQos1,
                                               ui->subscriptionQos2}));
    if (!subscription)
        return;
    connect(subscription,
            &QMqttSubscription::messageReceived,
            this,
            &QMqttViewer::handleMessageReceived);
    subscriptions->addSubscription(subscription);
    counts.insert(subscription, 0);
}

void QMqttViewer::loadSettings()
{
    QSettings settings;
    int size = settings.beginReadArray("Brokers");
    Broker broker;
    for (int i = 0; i < size; i++) {
        settings.setArrayIndex(i);
        broker.name = settings.value("name").toString();
        broker.address = settings.value("address").toString();
        broker.port = settings.value("port").toInt();
        broker.clientId = settings.value("clientId").toString();
        broker.keepAlive = settings.value("keepAlive").toInt();
        broker.cleanSession = settings.value("cleanSession").toBool();
        broker.autoReconnect = settings.value("autoReconnect").toBool();
        broker.mqttVersion = settings.value("mqttVersion").value<QMqttClient::ProtocolVersion>();
        broker.username = settings.value("username").toString();
        broker.password = settings.value("password").toString();
        broker.useSsl = settings.value("useSsl").toBool();
        broker.sslProtocol = settings.value("sslProtocol").value<QSsl::SslProtocol>();
        brokers->addBroker(broker);
    }
    settings.endArray();

    ui->broker->setCurrentIndex(settings.value("brokerIndex", 0).toInt());
    ui->publishTopic->setText(settings.value("pubTopic").toString());
    ui->subcribeTopic->setText(settings.value("subTopic").toString());
    ui->encoder->setCurrentText(settings.value("encoder").toString());
    ui->decoder->setCurrentText(settings.value("decoder").toString());
}

void QMqttViewer::saveSettings()
{
    QSettings settings;
    settings.beginWriteArray("Brokers", brokers->rowCount());
    for (int i = 0; i < brokers->rowCount(); i++) {
        settings.setArrayIndex(i);
        auto broker = brokers->get(i);
        settings.setValue("name", broker.name);
        settings.setValue("address", broker.address);
        settings.setValue("port", broker.port);
        settings.setValue("clientId", broker.clientId);
        settings.setValue("keepAlive", broker.keepAlive);
        settings.setValue("cleanSession", broker.cleanSession);
        settings.setValue("autoReconnect", broker.autoReconnect);
        settings.setValue("mqttVersion", broker.mqttVersion);
        settings.setValue("username", broker.username);
        settings.setValue("password", broker.password);
        settings.setValue("useSsl", broker.useSsl);
        settings.setValue("sslProtocol", broker.sslProtocol);
    }
    settings.endArray();

    saveUiSettings();
}

void QMqttViewer::saveUiSettings()
{
    QSettings settings;
    settings.setValue("brokerIndex", ui->broker->currentIndex());
    settings.setValue("pubTopic", ui->publishTopic->text());
    settings.setValue("subTopic", ui->subcribeTopic->text());
    settings.setValue("encoder", ui->encoder->currentText());
    settings.setValue("decoder", ui->decoder->currentText());
}

void QMqttViewer::handleConnected()
{
    ui->statusBar->showMessage(QStringLiteral("Connected"));
    qInfo() << "Connected to" << client->hostname();

    ui->connect->setText(tr("Disconnect"));
    for (const auto &widget : qAsConst(widgets))
        widget->setEnabled(true);
}

void QMqttViewer::handelDisconnected()
{
    ui->statusBar->showMessage(QStringLiteral("Disconnected"));
    qInfo() << "Disconnected to" << client->hostname();

    ui->connect->setText(tr("Connect"));
    for (const auto &widget : qAsConst(widgets))
        widget->setEnabled(false);

    subscriptions->clear();
    messages->clear();

    auto index = ui->broker->currentIndex();
    auto broker = brokers->get(index);
    if (broker.autoReconnect)
        handleConnect();
}

void QMqttViewer::handleError(QMqttClient::ClientError error)
{
    switch (error) {
    case QMqttClient::InvalidProtocolVersion:
        qWarning() << "Broker does not accept a connetion using the specified protocol version";
        break;
    case QMqttClient::NotAuthorized:
        qWarning() << "Authentication failed";
        break;
    case QMqttClient::TransportInvalid:
        qWarning() << "The underlying transport caused an error";
        break;
    default:
        qDebug() << error;
        break;
    }
}

void QMqttViewer::handleConnect()
{
    if (client->state() != QMqttClient::Disconnected) {
        qInfo() << "Disconnecting from host";
        client->disconnectFromHost();
        return;
    }
    qInfo() << "Attempting to connect to broker";

    auto index = ui->broker->currentIndex();
    auto broker = brokers->get(index);
    client->setHostname(broker.address);
    client->setPort(broker.port);
    client->setClientId(broker.clientId);

    client->setKeepAlive(broker.keepAlive);
    client->setCleanSession(broker.cleanSession);

    client->setUsername(broker.username);
    client->setPassword(broker.password);

    if (broker.useSsl) {
        QSslConfiguration conf;
        conf.setProtocol(broker.sslProtocol);
        client->connectToHostEncrypted(conf);
    } else {
        client->connectToHost();
    }
    ui->connect->setText(tr("Connecting"));
}

void QMqttViewer::handlePublish()
{
    auto topic = ui->publishTopic->text();
    if (topic.isEmpty())
        return;
    auto encoder = handlers[ui->encoder->currentData().toInt()];
    auto message = encoder->encodePayload(ui->payload->toPlainText());
    client->publish(topic,
                    message,
                    qos({ui->publishQos0, ui->publishQos1, ui->publishQos2}),
                    ui->retained->isChecked());
}

void QMqttViewer::handleSubscribe()
{
    subscribe(ui->subcribeTopic->text());
}

void QMqttViewer::handleMessageReceived(const QMqttMessage &message)
{
    auto subscription = qobject_cast<QMqttSubscription *>(sender());
    if (subscriptions->isMuted(subscription))
        return;
    Q_ASSERT(counts.contains(subscription));
    counts[subscription] += 1;
    messages->addMessage(message, counts[subscription]);
    connect(subscription,
            &QMqttSubscription::stateChanged,
            this,
            [this, message](QMqttSubscription::SubscriptionState state) {
                if (state == QMqttSubscription::Unsubscribed)
                    messages->removeMessage(message);
            });
}

void QMqttViewer::handleMessageDecoder(int index)
{
    Q_ASSERT(index >= 0 && index <= handlers.size());
    updateMessage(ui->messages->currentIndex());
}

void QMqttViewer::handleMessage(const QModelIndex &current, const QModelIndex &previous)
{
    updateMessage(current);
}

void QMqttViewer::clearMessages()
{
    messages->clear();
}

void QMqttViewer::scanTopics()
{
    if (!topicSubscription) {
        topicSubscription = client->subscribe(QMqttTopicFilter("#"));
        connect(topicSubscription,
                &QMqttSubscription::messageReceived,
                this,
                &QMqttViewer::handleTopicMessage);
        ui->scan->setText(tr("Stop"));
    } else {
        if (!subscriptions->contains("#"))
            topicSubscription->unsubscribe();
        topicSubscription = nullptr;
        ui->scan->setText(tr("Scan"));
    }
}

void QMqttViewer::handleTopicMessage(const QMqttMessage &message)
{
    topics->addTopic(message.topic().name());
}

void QMqttViewer::clearTopics()
{
    topics->clear();
}

void QMqttViewer::subscribeTopic()
{
    const auto selections = ui->topics->selectionModel()->selectedIndexes();
    if (selections.isEmpty())
        return;
    for (const auto &selection : selections) {
        auto topic = selection.data().toString();
        subscribe(topic);
    }
}

void QMqttViewer::updateMessage(const QModelIndex &index)
{
    auto decoder = handlers[ui->decoder->currentData().toInt()];
    auto message = decoder->decodePayload(
        messages->data(index, MessageModel::Message).toByteArray());

    ui->messageTopic->setText(messages->data(index, MessageModel::Topic).toString());
    auto time = messages->data(index, MessageModel::DateTime).toDateTime();
    ui->messageTime->setText(time.toString());
    ui->messageText->setText(message);
}

void QMqttViewer::openAddBrokerDialog()
{
    auto dialog = new BrokerDialog(brokers, this);
    dialog->setAttribute(Qt::WA_DeleteOnClose);
    dialog->setModal(true);
    dialog->show();
    connect(dialog, &BrokerDialog::destroyed, this, &QMqttViewer::saveSettings);
}

static void log(QtMsgType type, const QMessageLogContext &context, const QString &msg)
{
    QByteArray localMsg = msg.toLocal8Bit();
    const char *file = context.file ? context.file : "";
    const char *function = context.function ? context.function : "";
    switch (type) {
    case QtDebugMsg:
        fprintf(stderr,
                "Debug: %s (%s:%u, %s)\n",
                localMsg.constData(),
                file,
                context.line,
                function);
        break;
    case QtInfoMsg:
    case QtWarningMsg:
    case QtCriticalMsg:
    case QtFatalMsg:
        edit->appendPlainText(msg);
        break;
    }
}
