#ifndef ITEMWORKERGROUP_H
#define ITEMWORKERGROUP_H

#include <QObject>
#include "icdcore/icd_item.h"
#include "icdcore/icd_table.h"
#include "icdcore/icd_item_bit.h"
#include "icdcore/icd_item_frame.h"
#include "icdworker/icdworker.h"
#include "../coretree_widget.h"

class JTreeView;
class JTreeViewItem;

namespace Icd {

class ItemWorkerGroup : public QObject
{
    Q_OBJECT
public:
    explicit ItemWorkerGroup(QStandardItem *itemTable, const Icd::WorkerPtr &worker,
                             CoreTreeWidget::ShowAttributes attrs,
                             QObject *parent = 0);
    ~ItemWorkerGroup();

    void updateTimer();
    void updateItemData();
    void updateItemData(bool showValue);
    void updateItemData(CoreTreeWidget::ShowAttributes attrs, int dataFormat);
    int timerInterval() const;
    void setTimerInterval(int interval);
    void start();
    void start(int interval);
    void stop();
    bool isRunning() const;
    void setRunning(bool running);
    Icd::WorkerPtr worker() const;
    QStandardItem *itemTable() const;
    void setWorker(const WorkerPtr &worker);
    void setBindTableType(CoreTreeWidget::BindTableTypes type);
    void setDirty();

    static QString generateItemOffset(const ObjectPtr &object);

signals:

public slots:
    void onWorkerOpened();
    void onWorkerClosed();
    void onWorkerStarted();
    void onWorkerStopped();

protected:
    void timerEvent(QTimerEvent *event);

private:
    void updateItemData(QStandardItem *item, const Icd::FrameItemPtr &frame, bool showValue);
    void updateItemData(QStandardItem *item, const Icd::TablePtr &table, bool showValue);
    void updateItemData(QStandardItem *item, const Icd::ItemPtr &dataItem, bool showValue);
    void updateItemBitMap(QStandardItem *item, const Icd::BitItemPtr &bitItem, bool showValue);
    void updateFrameTable(QStandardItem *item, const Icd::TablePtr &table, bool show);

private:
    JTreeView *d_treeView;
    QStandardItem *d_itemTable;
    Icd::WorkerPtr d_worker;
    CoreTreeWidget::ShowAttributes d_showAttris;
    CoreTreeWidget::BindTableTypes d_bindTableTypes;
    int d_timerId;
    int d_timerInterval;
    int d_dataFormat;     // e.g.: 2 binary format
};

} // end of namespace Icd

#endif // ITEMWORKERGROUP_H
