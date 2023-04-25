#include "brokerdialog.h"
#include "ui_brokerdialog.h"

#include <QUuid>

BrokerDialog::BrokerDialog(BrokerModel *model, QWidget *parent) :
    QDialog(parent), model(model), ui(new Ui::BrokerDialog)
{
    ui->setupUi(this);
    ui->brokerPort->setValidator(new QIntValidator(this));
    ui->keepAlive->setValidator(new QIntValidator(this));
    ui->mqttVersion->addItem(QStringLiteral("3.1"), QMqttClient::MQTT_3_1);
    ui->mqttVersion->addItem(QStringLiteral("3.1.1"), QMqttClient::MQTT_3_1_1);
    ui->mqttVersion->addItem(QStringLiteral("5.0"), QMqttClient::MQTT_5_0);
    ui->sslProtocol->addItem(QStringLiteral("TLSv1.2"), QSsl::TlsV1_2);
    ui->sslProtocol->addItem(QStringLiteral("TLSv1.3"), QSsl::TlsV1_3);
    ui->splitter->setSizes({100, 250});

    ui->brokers->setModel(model);

    connect(ui->brokers->selectionModel(), &QItemSelectionModel::currentChanged, this, [this]() {
        auto index = ui->brokers->currentIndex();
        auto broker = this->model->get(index.row());
        setBroker(broker);
    });

    connect(ui->ok, &QPushButton::clicked, this, [this]() {
        updateBroker();
        accept();
    });
    connect(ui->cancel, &QPushButton::clicked, this, &BrokerDialog::reject);
    connect(ui->apply, &QPushButton::clicked, this, &BrokerDialog::updateBroker);
    connect(ui->generate, &QPushButton::clicked, this, [this]() {
        ui->clientId->setText(QUuid::createUuid().toString(QUuid::WithoutBraces));
    });
    connect(ui->add, &QPushButton::clicked, this, &BrokerDialog::addNewBroker);
    connect(ui->remove, &QPushButton::clicked, this, &BrokerDialog::removeSelectedBroker);
}

BrokerDialog::~BrokerDialog()
{
    delete ui;
}

Broker BrokerDialog::broker() const
{
    Broker broker;
    broker.name = ui->profileName->text();
    broker.address = ui->brokerAddress->text();
    broker.port = ui->brokerPort->text().toInt();
    broker.clientId = ui->clientId->text();

    broker.keepAlive = ui->keepAlive->text().toInt();
    broker.cleanSession = ui->cleanSession->isChecked();
    broker.autoReconnect = ui->autoReconnect->isChecked();

    broker.username = ui->username->text();
    broker.password = ui->password->text();

    broker.useSsl = ui->enableSsl->isChecked();
    broker.sslProtocol = ui->sslProtocol->itemData(ui->sslProtocol->currentIndex())
                             .value<QSsl::SslProtocol>();

    return broker;
}

void BrokerDialog::setBroker(const Broker &broker)
{
    ui->profileName->setText(broker.name);
    ui->brokerAddress->setText(broker.address);
    ui->brokerPort->setText(QString::number(broker.port));
    ui->clientId->setText(broker.clientId);

    ui->keepAlive->setText(QString::number(broker.keepAlive));
    ui->cleanSession->setChecked(broker.cleanSession);
    ui->autoReconnect->setChecked(broker.autoReconnect);
    if (broker.mqttVersion == QMqttClient::MQTT_3_1)
        ui->mqttVersion->setCurrentIndex(0);
    else if (broker.mqttVersion == QMqttClient::MQTT_3_1_1)
        ui->mqttVersion->setCurrentIndex(1);
    else if (broker.mqttVersion == QMqttClient::MQTT_5_0)
        ui->mqttVersion->setCurrentIndex(2);

    ui->username->setText(broker.username);
    ui->password->setText(broker.password);

    ui->enableSsl->setChecked(broker.useSsl);
    if (broker.sslProtocol == QSsl::TlsV1_2)
        ui->sslProtocol->setCurrentIndex(0);
    else if (broker.sslProtocol == QSsl::TlsV1_3)
        ui->sslProtocol->setCurrentIndex(1);
}

void BrokerDialog::addNewBroker()
{
    auto broker = Broker::defaultBroker();
    model->addBroker(broker);
}

void BrokerDialog::removeSelectedBroker()
{
    auto index = ui->brokers->currentIndex();
    model->removeRow(index.row());
}

void BrokerDialog::updateBroker()
{
    auto index = ui->brokers->currentIndex();
    auto broker = this->broker();
    model->update(index, broker);
}
