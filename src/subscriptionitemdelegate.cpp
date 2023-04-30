#include "subscriptionitemdelegate.h"
#include "subscriptionmodel.h"

#include <QApplication>
#include <QMouseEvent>
#include <QPainter>

enum SubscriptionElement {
    SE_Unsubscribe,
};

static QRect elementRect(SubscriptionElement element, const QStyleOption *opt);
static void initButtonOption(QStyleOptionButton &opt, const QPushButton *button);

SubscriptionItemDelegate::SubscriptionItemDelegate(QObject *parent) :
    QStyledItemDelegate(parent), unsubscribe(new QPushButton("Unsubscribe"))
{
    unsubscribe->resize(100, 20);
}

SubscriptionItemDelegate::~SubscriptionItemDelegate()
{
    delete unsubscribe;
}

QWidget *SubscriptionItemDelegate::createEditor(QWidget *parent,
                                                const QStyleOptionViewItem &option,
                                                const QModelIndex &index) const
{
    return QStyledItemDelegate::createEditor(parent, option, index);
}

void SubscriptionItemDelegate::setEditorData(QWidget *editor, const QModelIndex &index) const
{
    QStyledItemDelegate::setEditorData(editor, index);
}

void SubscriptionItemDelegate::setModelData(QWidget *editor,
                                            QAbstractItemModel *model,
                                            const QModelIndex &index) const
{
    QStyledItemDelegate::setModelData(editor, model, index);
}

void SubscriptionItemDelegate::updateEditorGeometry(QWidget *editor,
                                                    const QStyleOptionViewItem &option,
                                                    const QModelIndex &index) const
{
    QStyledItemDelegate::updateEditorGeometry(editor, option, index);
}

bool SubscriptionItemDelegate::editorEvent(QEvent *event,
                                           QAbstractItemModel *model,
                                           const QStyleOptionViewItem &option,
                                           const QModelIndex &index)
{
    switch (event->type()) {
    case QEvent::MouseButtonRelease: {
        auto mouse = static_cast<QMouseEvent *>(event);
        if (!mouse)
            break;
        auto unsubscribedRect = elementRect(SE_Unsubscribe, &option);
        if (unsubscribedRect.contains(mouse->pos())) {
            auto subModel = qobject_cast<SubscriptionModel *>(model);
            subModel->removeRow(index.row());
            break;
        }
    } break;
    default:
        break;
    }

    return QStyledItemDelegate::editorEvent(event, model, option, index);
}

void SubscriptionItemDelegate::paint(QPainter *painter,
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
        auto buttonRect = elementRect(SE_Unsubscribe, &option);
        unsubscribe->move(buttonRect.topLeft());
        QStyleOptionButton buttonOpt;
        initButtonOption(buttonOpt, unsubscribe);
        buttonOpt.rect = buttonRect;
        style->drawControl(QStyle::CE_PushButton, &buttonOpt, painter, nullptr);
    }
}

QSize SubscriptionItemDelegate::sizeHint(const QStyleOptionViewItem &option,
                                         const QModelIndex &index) const
{
    auto styled = QStyledItemDelegate::sizeHint(option, index);
    styled.setHeight(50);
    return styled;
}

static QRect elementRect(SubscriptionElement element, const QStyleOption *opt)
{
    switch (element) {
    case SE_Unsubscribe: {
        auto margin = opt->rect.marginsRemoved(QMargins(3, 3, 3, 3));
        QRect widgetRect(QPoint(), elementSize(element));
        widgetRect.moveBottomRight(margin.bottomRight());
        return widgetRect;
    }
    default:
        return {};
    }
}

static void initButtonOption(QStyleOptionButton &opt, const QPushButton *button)
{
    opt.initFrom(button);
    if (button->autoDefault())
        opt.features |= QStyleOptionButton::AutoDefaultButton;
    if (button->isDefault())
        opt.features |= QStyleOptionButton::DefaultButton;
    if (button->isDown())
        opt.state |= QStyle::State_Sunken;
    if (button->isChecked())
        opt.state |= QStyle::State_On;
    if (!button->isFlat() && !button->isDown())
        opt.state |= QStyle::State_Raised;
    opt.text = button->text();
}
