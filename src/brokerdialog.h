#pragma once

#include "broker.h"
#include "brokermodel.h"

#include <QDialog>

namespace Ui {
class BrokerDialog;
}

class BrokerDialog : public QDialog
{
    Q_OBJECT

public:
    explicit BrokerDialog(BrokerModel *model, QWidget *parent = nullptr);
    ~BrokerDialog();

    Broker broker() const;

private slots:
    void setBroker(const Broker &broker);
    void addNewBroker();
    void removeSelectedBroker();
    void updateBroker();

private:
    BrokerModel *model;
    Ui::BrokerDialog *ui;
};
