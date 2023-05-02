#pragma once

#include <QPushButton>
#include <QStyledItemDelegate>

class SubscriptionItemDelegate : public QStyledItemDelegate
{
    Q_OBJECT
public:
    SubscriptionItemDelegate(QObject *parent = nullptr);

    ~SubscriptionItemDelegate();

    bool editorEvent(QEvent *event,
                     QAbstractItemModel *model,
                     const QStyleOptionViewItem &option,
                     const QModelIndex &index) override;

    void paint(QPainter *painter,
               const QStyleOptionViewItem &option,
               const QModelIndex &index) const override;

    QSize sizeHint(const QStyleOptionViewItem &option, const QModelIndex &index) const override;

private:
    QPushButton *mute;
    QPushButton *unsubscribe;
};
