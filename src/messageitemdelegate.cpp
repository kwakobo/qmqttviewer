#include "messageitemdelegate.h"
#include "messagemodel.h"

#include <QApplication>
#include <QFontMetrics>
#include <QPainter>

static void drawLabel(QPainter *painter, const QStyleOptionViewItem &opt);

MessageItemDelegate::MessageItemDelegate(QObject *parent) : QStyledItemDelegate(parent) {}

void MessageItemDelegate::paint(QPainter *painter,
                                const QStyleOptionViewItem &option,
                                const QModelIndex &index) const
{
    QStyleOptionViewItem opt = option;
    initStyleOption(&opt, index);
    opt.displayAlignment = Qt::AlignTop | Qt::AlignLeft;

    const QWidget *widget = opt.widget;
    QStyle *style = widget ? widget->style() : QApplication::style();
    style->drawControl(QStyle::CE_ItemViewItem, &opt, painter, widget);

    {
        auto qosOpt = opt;
        auto qos = index.data(MessageModel::Count).toInt();
        qosOpt.text = QString::number(qos);
        qosOpt.rect = opt.fontMetrics.boundingRect(qosOpt.text).marginsAdded(QMargins(6, 0, 6, 0));
        qosOpt.rect.moveTopRight(opt.rect.marginsRemoved(QMargins(2, 2, 2, 2)).topRight());
        drawLabel(painter, qosOpt);
    }
    {
        auto qosOpt = opt;
        auto qos = index.data(MessageModel::Qos).toInt();
        qosOpt.text = QStringLiteral("QoS %1").arg(QString::number(qos));
        qosOpt.rect = opt.fontMetrics.boundingRect(qosOpt.text).marginsAdded(QMargins(6, 0, 6, 0));
        qosOpt.rect.moveBottomRight(opt.rect.marginsRemoved(QMargins(3, 3, 3, 3)).bottomRight());
        drawLabel(painter, qosOpt);
    }
}

QSize MessageItemDelegate::sizeHint(const QStyleOptionViewItem &option,
                                    const QModelIndex &index) const
{
    auto styled = QStyledItemDelegate::sizeHint(option, index);
    styled.setHeight(40);
    return styled;
}

static void drawLabel(QPainter *painter, const QStyleOptionViewItem &opt)
{
    painter->save();
    QColor color(197, 213, 255);
    painter->setPen(color.darker());
    painter->setBrush(color);
    painter->drawRect(opt.rect);
    painter->restore();
    painter->save();
    painter->setFont(opt.font);
    painter->drawText(opt.rect, opt.text, {Qt::AlignCenter});
    painter->restore();
}
