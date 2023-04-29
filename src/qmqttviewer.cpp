#include "qmqttviewer.h"
#include "brokerdialog.h"
#include "ui_qmqttviewer.h"

#include "decoder/base64decoder.h"
#include "decoder/hexmessagedecoder.h"
#include "decoder/jsonprettymessagedecoder.h"
#include "decoder/plainmessagedecoder.h"
#include "decoder/sparkplugmessagedecoder.h"

#include <QButtonGroup>
#include <QSettings>

#include <algorithm>

static QPlainTextEdit *edit = nullptr;

static void log(QtMsgType type, const QMessageLogContext &context, const QString &msg);

QMqttViewer::QMqttViewer(QWidget *parent) :
    QMainWindow(parent), ui(new Ui::QMqttViewer), client(new QMqttClient(this)),
    brokers(new BrokerModel(this)), messages(new MessageModel(this)),
    subscriptions(new SubscriptionModel(this)), decoders({
                                                    new PlainMessageDecoder,
                                                    new JsonPrettyMessageDecoder,
                                                    new Base64Decoder,
                                                    new HexMessageDecoder,
                                                    new SparkplugMessageDecoder,
                                                })
{
    ui->setupUi(this);

    for (int i = 0; i < decoders.size(); i++)
        ui->decoder->insertItem(i, decoders[i]->displayName());

    ui->broker->setModel(brokers);
    ui->messages->setModel(messages);
    ui->subscriptions->setModel(subscriptions);

    auto publishQos = new QButtonGroup(this);
    publishQos->addButton(ui->publishQos0);
    publishQos->addButton(ui->publishQos1);
    publishQos->addButton(ui->publishQos2);

    auto subscribeQos = new QButtonGroup(this);
    subscribeQos->addButton(ui->subscriptionQos0);
    subscribeQos->addButton(ui->subscriptionQos1);
    subscribeQos->addButton(ui->subscriptionQos2);

    connect(ui->exit, &QAction::triggered, qApp, &QApplication::exit);
    connect(ui->editConnections, &QAction::triggered, this, &QMqttViewer::openAddBrokerDialog);
    connect(ui->about, &QAction::triggered, qApp, &QApplication::aboutQt);
    connect(ui->connect, &QPushButton::clicked, this, &QMqttViewer::handleConnect);
    connect(ui->publish, &QPushButton::clicked, this, &QMqttViewer::handlePublish);
    connect(ui->subscribe, &QPushButton::clicked, this, &QMqttViewer::handleSubscribe);
    connect(ui->decoder, &QComboBox::activated, this, &QMqttViewer::handleMessageDecoder);

    connect(ui->messages->selectionModel(),
            &QItemSelectionModel::currentChanged,
            this,
            &QMqttViewer::handleMessage);
    connect(messages, &MessageModel::rowsInserted, ui->messages, [this]() {
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
    delete client;
    delete ui;
    qDeleteAll(decoders);
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

static int qos(const QList<QCheckBox *> &checkboxes)
{
    for (int i = 0; i < checkboxes.size(); i++) {
        if (checkboxes[i]->isChecked())
            return i;
    }
    return 0;
}

void QMqttViewer::handlePublish()
{
    auto topic = ui->publishTopic->text();
    if (topic.isEmpty())
        return;
    client->publish(topic,
                    ui->payload->toPlainText().toLocal8Bit(),
                    qos({ui->publishQos0, ui->publishQos1, ui->publishQos2}),
                    ui->retained->isChecked());
}

void QMqttViewer::handleSubscribe()
{
    auto topic = ui->subcribeTopic->text();
    if (topic.isEmpty() || subscriptions->contains(topic))
        return;
    auto subscription = client->subscribe(topic,
                                          qos({ui->subscriptionQos0,
                                               ui->subscriptionQos1,
                                               ui->subscriptionQos2}));
    connect(subscription,
            &QMqttSubscription::messageReceived,
            this,
            &QMqttViewer::handleMessageReceived);
    subscriptions->addSubscription(subscription);
}

void QMqttViewer::handleMessageReceived(const QMqttMessage &message)
{
    messages->addMessage(message);
}

void QMqttViewer::handleMessageDecoder(int index)
{
    Q_ASSERT(index >= 0 && index <= decoders.size());
    updateMessage(ui->messages->currentIndex());
}

void QMqttViewer::handleMessage(const QModelIndex &current, const QModelIndex &previous)
{
    updateMessage(current);
}

void QMqttViewer::updateMessage(const QModelIndex &index)
{
    auto decoder = decoders[ui->decoder->currentIndex()];
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
