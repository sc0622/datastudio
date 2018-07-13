#ifndef CORETREE_WIDGET_P_H
#define CORETREE_WIDGET_P_H

#include "jwt/jtreeview.h"
#include "../coretree_widget.h"
#include "icdcore/icdcore_inc.h"
#include "icdparser/icdparser.h"
#include "icdworker/icdworker.h"
#include <functional>
#include <QFutureWatcher>

class QLabel;
class QSpacerItem;
class QPushButton;
class QStandardItemModel;

namespace Icd {

// class TreeItemDelegate

class TreeItemDelegateData;

class TreeItemDelegate : public JTreeItemDelegate
{
    Q_OBJECT
public:
    explicit TreeItemDelegate(JTreeView *treeView, QObject *parent = Q_NULLPTR);
    ~TreeItemDelegate();

    QSize sizeHint(const QStyleOptionViewItem &option,
                   const QModelIndex &index) const Q_DECL_OVERRIDE;
    void paint(QPainter *painter, const QStyleOptionViewItem &option,
               const QModelIndex &index) const Q_DECL_OVERRIDE;

private:
    static QStandardItem *tableItem(const QModelIndex &index);

private:
    TreeItemDelegateData *d;
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

public slots:
    void setSpacing(int spacing);

private:
    void updateButtonIcon(bool checked);

private:
    QSpacerItem *spacer_;
    QPushButton *buttonRun_;
    Icd::WorkerPtr worker_;
    CoreTreeWidget::BindTableTypes bindTableTypes_;
};

// class CoreTreeWidgetPrivate

class ItemWorkerGroup;
class SearchEdit;
struct BindingData;

class CoreTreeWidgetPrivate : public JTreeView
{
    Q_OBJECT
    Q_PROPERTY(quint32 bindTableTypes READ bindTableTypes NOTIFY bindTableTypesChanged)
    Q_PROPERTY(quint32 showAttris READ showAttris NOTIFY showAttrisChanged)
    Q_PROPERTY(QColor valueColor READ valueColor WRITE setValueColor NOTIFY valueColorChanged)
public:
    explicit CoreTreeWidgetPrivate(CoreTreeWidget *q);
    ~CoreTreeWidgetPrivate();

    quint32 bindTableTypes() const { return quint32(bindTableTypes_); }
    quint32 showAttris() { return quint32(showAttris_); }
    QColor valueColor() const;

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
    void exportTableData(QStandardItem *item);
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
    void bindTableTypesChanged();
    void showAttrisChanged();
    void valueColorChanged(const QColor &color);

    void itemUnloaded(QStandardItem *item, QStandardItem *itemTable);
    void channelBound(QStandardItem *item, const QString &channelId);
    void channelUnbound(QStandardItem *item, const QString &channelId);
    void channelChanged(QStandardItem *item, const QString &channelId);
    void unbindItem(QStandardItem *item, QStandardItem *itemTable);

public slots:
    void setValueColor(const QColor &color);

    void onTreeItemPressed(QStandardItem *item);
    void onWorkerRemoved(const Icd::WorkerPtr &worker);
    void onWorkerCleared();

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

    static bool loadItem(QObject *target, QStandardItem *itemTable, const Icd::ItemPtrArray &items, int deep);

    static QStandardItem *loadTable(QObject *target, QStandardItem *itemDataItem, const Icd::TablePtr &table);
    static bool loadItem(QObject *target, QStandardItem *itemTable, const Icd::ItemPtr &item);
    static bool loadFrameCodeItem(QObject *target, QStandardItem *itemTable, const Icd::FrameCodeItemPtr &frameCodeItem);
    static bool loadComplexItem(QObject *target, QStandardItem *itemDataItem, const Icd::ComplexItemPtr &complexItem);
    static bool loadFrameItem(QObject *target, QStandardItem *itemDataItem, const Icd::FrameItemPtr &frameItem);
    static bool loadBitItem(QObject *target, QStandardItem *itemDataItem, const Icd::BitItemPtr &bitItem);

    //
    void removeTableItem(QStandardItem *item);

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

    void restoreChannelItem(QStandardItem *itemTable, const TablePtr &table);
    //
    QStandardItem *itemFromAction(QObject *action) const;
    QVariant varFromAction(QObject *action, const char *name) const;

    QStandardItem *findItemByDomain(QStandardItem *parentItem, const QString &domain, int domainType);
    void selectItem(const QString &domain, int domainType);
    QString itemDomain(QStandardItem *item, int domainType) const;

    ///

    static QString idDomain(QStandardItem *item);
    static QString markDomain(QStandardItem *item);
    static bool loadTable(JTreeView *treeView, QStandardItem *itemParent, const Icd::TablePtr &table, int deep);

private:
    J_DECLARE_PUBLIC(CoreTreeWidget)
    SearchEdit *searchEdit_;
    Icd::ParserPtr parser_;
    int loadingDeep_;
    int intervalUpdate_;
    CoreTreeWidget::BindTableTypes bindTableTypes_;
    CoreTreeWidget::ShowAttributes showAttris_;
    int dataFormat_;
    CoreTreeWidget::TreeModes treeModes_;
    QHash<QStandardItem * /*itemTable*/, ItemWorkerGroup*> workerGroups_;
    QFutureWatcher<BindingData> watcher_;

    // qss
    QColor valueColor_;
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
