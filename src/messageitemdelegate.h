#pragma once

#include <QStyledItemDelegate>

class MessageItemDelegate : public QStyledItemDelegate
{
    Q_OBJECT
public:
    MessageItemDelegate(QObject *parent = nullptr);

    void paint(QPainter *painter,
               const QStyleOptionViewItem &option,
               const QModelIndex &index) const override;

    QSize sizeHint(const QStyleOptionViewItem &option, const QModelIndex &index) const override;
};
