#ifndef CORETREE_WIDGET_P_H
#define CORETREE_WIDGET_P_H

#include "jwt/jtreeview.h"
#include "../coretree_widget.h"
#include "icdcore/icdcore_inc.h"
#include "icdparser/icdparser.h"
#include "icdworker/icdworker.h"
#include <functional>

template <> class QFutureWatcher<void>;

class QLabel;
class QPushButton;

namespace Icd {

// class TreeItemDelegate

class TreeItemDelegate : public JTreeItemDelegate
{
    Q_OBJECT
public:
    explicit TreeItemDelegate(QObject *parent = Q_NULLPTR);

    QSize sizeHint(const QStyleOptionViewItem &option,
                   const QModelIndex &index) const Q_DECL_OVERRIDE;
};

// class TableItemWidget

class TableItemWidget : public QWidget
{
    Q_OBJECT
public:
    explicit TableItemWidget(CoreTreeWidget::BindTableTypes bindingTypes,
                             QWidget *parent = nullptr);
    ~TableItemWidget();

    Icd::WorkerPtr worker() const;
    void setWorker(const Icd::WorkerPtr &worker);

    void start();
    void stop();
    bool toggle(bool checked);
    bool isRunning() const;

signals:
    void clicked();
    void toggled(bool checked);

private:
    void updateButtonIcon(bool checked);

private:
    QPushButton *d_buttonRun;
    Icd::WorkerPtr d_worker;
    CoreTreeWidget::BindTableTypes d_bindTableTypes;
};

// class CoreTreeWidgetPrivate

class ItemWorkerGroup;
class SearchEdit;
struct BindingData;

class CoreTreeWidgetPrivate : public JTreeView
{
    Q_OBJECT
public:
    explicit CoreTreeWidgetPrivate(CoreTreeWidget *q);
    ~CoreTreeWidgetPrivate();

    void init();
    void setParser(const Icd::ParserPtr &parser);
    int loadingDeep() const;
    void setLoadingDeep(int deep);
    int intervalUpdate() const;
    void setIntervalUpdate(int interval);
    bool loadData();
    bool loadData(const Icd::TablePtr &table, const QString &domain);
    bool loadData(const Icd::TablePtr &table, const QString &filePath,
                  bool hasTimeFormat, int headerSize, const QString &domain);
    void clearData();

    CoreTreeWidget::ShowAttributes showAttributes() const;
    void setShowAttributes(CoreTreeWidget::ShowAttributes attrs);
    void setShowAttribute(CoreTreeWidget::ShowAttribute attr, bool on = true);
    bool testShowAttribute(CoreTreeWidget::ShowAttribute attr) const;

    int dataFormat() const;
    void setDataFormat(int format);

    CoreTreeWidget::TreeModes treeModes() const;
    void setTreeMode(CoreTreeWidget::TreeModes modes);
    void setTreeMode(CoreTreeWidget::TreeMode mode, bool on = true);
    bool testTreeMode(CoreTreeWidget::TreeMode mode) const;

    bool isRunning() const;
    void setRunning(bool value);

    // batch process
    void bindingChannels(const QString &filePath);
    void unbindingChannels();
    void exportBindingStatus(const QString &filePath);
    void runAllChannels();
    void stopAllChannels();

public:
    Q_INVOKABLE bool changeChannel(QStandardItem *itemTable);
    Q_INVOKABLE bool bindChannel(QStandardItem *itemTable);
    Q_INVOKABLE bool bindChannel(QStandardItem *itemTable, const Icd::WorkerPtr &worker);
    Q_INVOKABLE bool bindChannel(QStandardItem *itemTable, const Icd::WorkerPtr &worker,
                                 const Icd::TablePtr &table);
    Q_INVOKABLE bool unbindChannel(QStandardItem *itemTable);
    Q_INVOKABLE bool unbindChannelItem(QStandardItem *item);
    Q_INVOKABLE bool unbindChannel(QStandardItem *itemTable, const Icd::WorkerPtr &worker);
    Q_INVOKABLE bool isBoundChannel(QStandardItem *itemTable) const;

signals:
    void itemUnloaded(QStandardItem *item, QStandardItem *itemTable);
    void channelBound(QStandardItem *item, const QString &channelId);
    void channelUnbound(QStandardItem *item, const QString &channelId);
    void channelChanged(QStandardItem *item, const QString &channelId);
    void unbindItem(QStandardItem *item, QStandardItem *itemTable);

public slots:
    void onTreeItemPressed(QStandardItem *item);

    void onWorkerRemoved(const Icd::WorkerPtr &worker);
    void onWorkerCleared();

    // menu
    void onActionExpandItemAll();
    void onActionCollapseItemAll();
    void onActionLoadVehicle();
    void onActionUpdateVehicle();
    void onActionUnloadVehicle();
    void onActionExportAllData();
    void onActionExportExistData();
    void onActionLoadSystem();
    void onActionUpdateSystem();
    void onActionUnloadSystem();
    void onActionLoadTable();
    void onActionUpdateTable();
    void onActionUnloadTable();
    void onActionDeleteDataItem();
    void onActionDeleteAllView();
    void onActionLoadDataItem();
    void onActionUpdateDataItem();
    void onActionUnloadDataItem();
    void onActionChangeChannelBound();
    void onActionBindChannel();
    void onActionUnboundChannel();
    void onActionStartRecordData();
    void onActionStopRecordData();
    void onActionDeleteTableView();
    void onActionExportTableData();

protected:
    QStringList mimeTypes() const;
    QMimeData *mimeData(const QList<QStandardItem *> &items) const;

private:
    void itemRootRightClicked(QStandardItem *item, int deep);
    void itemVehicleRightClicked(QStandardItem *item, int deep);
    void itemSystemRightClicked(QStandardItem *item, int deep);
    void itemTableRightClicked(QStandardItem *item, int deep);
    void itemDataItemRightClicked(QStandardItem *item, int deep);
    void itemItemTableRightClicked(QStandardItem *item, int deep);
    void itemItemBitMapRightClicked(QStandardItem *item, int deep);

private:
    bool loadVehicle(QStandardItem *itemRoot, int deep);
    bool loadSystem(QStandardItem *itemVehicle, int deep);
    bool loadSystem(QStandardItem *itemVehicle, const Icd::SystemPtrArray &systems, int deep);
    bool loadTable(QStandardItem *itemSystem, int deep);
    bool loadTable(QStandardItem *itemSystem, const Icd::TablePtrArray &tables, int deep);
    bool loadItem(QStandardItem *itemTable, int deep);
    bool loadItem(QStandardItem *itemTable, const Icd::ItemPtrArray &items, int deep);

    QStandardItem *loadTable(QStandardItem *itemDataItem, const Icd::TablePtr &table);
    bool loadItem(QStandardItem *itemTable, const Icd::ItemPtr &item);
    bool loadFrameCodeItem(QStandardItem *itemTable, const Icd::FrameCodeItemPtr &frameCodeItem);
    bool loadComplexItem(QStandardItem *itemDataItem, const Icd::ComplexItemPtr &complexItem);
    bool loadFrameItem(QStandardItem *itemDataItem, const Icd::FrameItemPtr &frameItem);
    bool loadBitItem(QStandardItem *itemDataItem, const Icd::BitItemPtr &bitItem);

    //
    void removeTableItem(QStandardItem *item);
    void clearChildren(QStandardItem *item);
    void expandItem(QStandardItem *item, bool expanded, int deep);

    Icd::WorkerPtr queryWorker(const QStandardItem *itemTable) const;

    void updateItemData(QStandardItem *item);
    void updateBitItemData(QStandardItem *itemDataItem, const Icd::BitItemPtr &bitItem);
    void updateItemData(QStandardItem *itemDataItem, const Icd::ItemPtr &dataItem);
    void updateItemData(QStandardItem *itemTable, const Icd::ItemPtrArray &dataItems);
    void updateItemData(QStandardItem *itemDataItem, const Icd::ComplexItemPtr &complex);
    void updateItemData(QStandardItem *itemDataItem, const Icd::FrameItemPtr &frame);
    void updateItemData(QStandardItem *itemTable, const Icd::TablePtr &table);
    void updateItemData(QStandardItem *itemTable, bool showValue);

    void removeWorkerGroup(QStandardItem *itemTable);
    void removeWorkerGroup(const Icd::WorkerPtr &worker);
    void clearWorkerGroup();
    ItemWorkerGroup *findWorkerGroup(QStandardItem *itemTable) const;

    bool hasItemBound(QStandardItem *item);
    void clearItemBoundRole(QStandardItem *item, bool bEmit);

    QStandardItem *findItemTable(QStandardItem *item, const QString &filePath) const;
    QStandardItem *findItemTable(QStandardItem *item) const;

    bool exportData(const QStandardItem *item, bool exportAll);

    // batch
    QStandardItem *findTableItem(QStandardItem *itemParent, const QString &domain);
    void findAllTableItem(QList<QStandardItem *> &items,
                          QStandardItem *itemParent, bool binding);
    void procAllTableItem(QStandardItem *itemParent, bool binding,
                          std::function<bool(QStandardItem *, const QString &)> callback);

    static BindingData bindingMapTask(BindingData data);
    static void bindingMapReduce(int &count, const BindingData &data);

    //
    QStandardItem *itemFromAction(QObject *action) const;
    QVariant varFromAction(QObject *action, const char *name) const;

private:
    J_DECLARE_PUBLIC(CoreTreeWidget)
    SearchEdit *d_searchEdit;
    Icd::ParserPtr d_parser;
    int d_loadingDeep;
    int d_intervalUpdate;
    CoreTreeWidget::BindTableTypes d_bindTableTypes;
    CoreTreeWidget::ShowAttributes d_showAttris;
    int d_dataFormat;
    CoreTreeWidget::TreeModes d_treeModes;
    QHash<QStandardItem * /*itemTable*/, ItemWorkerGroup*> d_workerGroups;
    QFutureWatcher<BindingData> d_watcher;
};

//
struct BindingData
{
    QString channelId;
    QString tableDomain;
    CoreTreeWidgetPrivate *d;
    QStandardItem *item;
    Icd::WorkerPtr worker;
    Icd::TablePtr table;

    BindingData():d(Q_NULLPTR){}
    BindingData(const QString &channelId, const QString &tableDomain,
                CoreTreeWidgetPrivate *d)
        : channelId(channelId)
        , tableDomain(tableDomain)
        , d(d)
    {

    }

    BindingData(const BindingData &other)
    {
        *this = other;
    }

    BindingData &operator =(const BindingData &other)
    {
        if (this == &other) {
            return *this;
        }
        channelId = other.channelId;
        tableDomain = other.tableDomain;
        d = other.d;
        item = other.item;
        worker = other.worker;
        table = other.table;
        return *this;
    }
};

Q_DECLARE_METATYPE(BindingData)

} // end of namespace Icd

#endif // CORETREE_WIDGET_P_H
