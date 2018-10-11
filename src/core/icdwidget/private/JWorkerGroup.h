#ifndef JWORKERGROUP_H
#define JWORKERGROUP_H

#include <QObject>
#include "icdcore/icd_item.h"
#include "icdcore/icd_table.h"
#include "icdcore/icd_item_bit.h"
#include "icdcore/icd_item_frame.h"
#include "icdworker/icdworker.h"
#include "../JProtoTreeView.h"

class JTreeView;
class JTreeViewItem;

namespace Icd {

class JProtoTreeViewPrivate;

class JWorkerGroup : public QObject
{
    Q_OBJECT
public:
    explicit JWorkerGroup(QStandardItem *itemTable, const Icd::WorkerPtr &worker,
                          JProtoTreeView::ShowAttributes attrs,
                          QObject *parent = nullptr);
    ~JWorkerGroup();

    void updateTimer();
    void updateItemData();
    void updateItemData(bool showValue);
    void updateItemData(JProtoTreeView::ShowAttributes attrs, int dataFormat);
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
    void setBindTableType(JProtoTreeView::BindTableTypes type);
    void setDirty();

    static QString generateItemOffset(Icd::Object *object, int offset = 0);

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
    void updateFrameTable(QStandardItem *item, const Icd::TablePtr &table, bool show, int index = 0);

private:
    JProtoTreeViewPrivate *treeView_;
    QStandardItem *itemTable_;
    Icd::WorkerPtr worker_;
    JProtoTreeView::ShowAttributes showAttris_;
    JProtoTreeView::BindTableTypes bindTableTypes_;
    int timerId_;
    int timerInterval_;
    int dataFormat_;     // e.g.: 2 binary format
};

} // end of namespace Icd

#endif // JWORKERGROUP_H
