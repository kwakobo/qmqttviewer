#include "subscriptionitemdelegate.h"
#include "subscriptionmodel.h"

#include <QApplication>
#include <QMouseEvent>
#include <QPainter>

enum SubscriptionElement {
    SE_Mute,
    SE_Unsubscribe,
};

static QSize elementSize(SubscriptionElement element);
static QRect elementRect(SubscriptionElement element, const QStyleOption *opt);
static void initButtonOption(QStyleOptionButton &opt, const QPushButton *button);

static const int MARGIN = 3;
static const int SPACING = 6;

SubscriptionItemDelegate::SubscriptionItemDelegate(QObject *parent) :
    QStyledItemDelegate(parent), mute(new QPushButton("Mute")),
    unsubscribe(new QPushButton("Unsubscribe"))
{
    mute->resize(elementSize(SE_Mute));
    unsubscribe->resize(elementSize(SE_Unsubscribe));
}

SubscriptionItemDelegate::~SubscriptionItemDelegate()
{
    delete mute;
    delete unsubscribe;
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
        auto muteRect = elementRect(SE_Mute, &option);
        if (muteRect.contains(mouse->pos())) {
            auto subModel = qobject_cast<SubscriptionModel *>(model);
            subModel->toggleMute(index);
            return true;
        }
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
        auto buttonRect = elementRect(SE_Mute, &option);
        mute->move(buttonRect.topLeft());
        auto model = qobject_cast<const SubscriptionModel *>(index.model());
        QStyleOptionButton buttonOpt;
        initButtonOption(buttonOpt, mute);
        if (model->isMuted(index))
            buttonOpt.text = tr("Unmute");
        else
            buttonOpt.text = tr("Mute");
        buttonOpt.rect = buttonRect;
        style->drawControl(QStyle::CE_PushButton, &buttonOpt, painter, nullptr);
    }
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

static QSize elementSize(SubscriptionElement element)
{
    switch (element) {
    case SE_Mute:
        return {80, 20};
    case SE_Unsubscribe:
        return {100, 20};
    default:
        return {};
    }
}

static QRect elementRect(SubscriptionElement element, const QStyleOption *opt)
{
    switch (element) {
    case SE_Mute: {
        auto margin = opt->rect.marginsRemoved(QMargins(MARGIN, MARGIN, MARGIN, MARGIN));
        auto unsubscribeRect = elementRect(SE_Unsubscribe, opt);
        margin.adjust(0, 0, -unsubscribeRect.width() - SPACING, 0);
        QRect widgetRect(QPoint(), elementSize(element));
        widgetRect.moveBottomRight(margin.bottomRight());
        return widgetRect;
    }
    case SE_Unsubscribe: {
        auto margin = opt->rect.marginsRemoved(QMargins(MARGIN, MARGIN, MARGIN, MARGIN));
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
