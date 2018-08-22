#ifndef JCHANNEL_PANE_P_H
#define JCHANNEL_PANE_P_H

#include "../JChannelPane.h"
#include "jwt/jtableview.h"

namespace Icd {

// class JChannelPanePrivate

class JChannelPanePrivate
{
public:
    explicit JChannelPanePrivate(JChannelPane *q);
    ~JChannelPanePrivate();

    void init();
    void moveItemToBottom(QStandardItem *item);
    void swapItem(QStandardItem *item1, QStandardItem *item2);

private:
    J_DECLARE_PUBLIC(JChannelPane)
    JTableView *tableView;
};

} // end of namespace Icd

#endif // JCHANNEL_PANE_P_H
