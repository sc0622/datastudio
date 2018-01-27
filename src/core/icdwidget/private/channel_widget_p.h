#ifndef CHANNEL_WIDGET_P_H
#define CHANNEL_WIDGET_P_H

#include "../channel_widget.h"
#include "jtableview.h"

namespace Icd {

// class ChannelWidgetPrivate

class ChannelWidgetPrivate
{
public:
    explicit ChannelWidgetPrivate(ChannelWidget *q);
    ~ChannelWidgetPrivate();

    void init();
    void moveItemToBottom(QStandardItem *item);
    void swapItem(QStandardItem *item1, QStandardItem *item2);

private:
    J_DECLARE_PUBLIC(ChannelWidget)
    JTableView *tableView;
};

} // end of namespace Icd

#endif // CHANNEL_WIDGET_P_H
