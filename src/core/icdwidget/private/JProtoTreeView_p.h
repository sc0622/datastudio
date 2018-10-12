﻿#ifndef JPROTOTREEVIEW_P_H
#define JPROTOTREEVIEW_P_H

#include "jwt/jtreeview.h"
#include "../JProtoTreeView.h"
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
    explicit TreeItemDelegate(JTreeView *treeView, QObject *parent = nullptr);
    ~TreeItemDelegate() override;

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
    explicit TableItemWidget(JProtoTreeView::BindTableTypes bindingTypes,
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
    JProtoTreeView::BindTableTypes bindTableTypes_;
};

// class JProtoTreeViewPrivate

class JWorkerGroup;
class JSearchEdit;
struct BindingData;

class JProtoTreeViewPrivate : public JTreeView
{
    Q_OBJECT
    Q_PROPERTY(quint32 bindTableTypes READ bindTableTypes NOTIFY bindTableTypesChanged)
    Q_PROPERTY(quint32 treeModes READ treeModes NOTIFY treeModesChanged)
    Q_PROPERTY(quint32 showAttris READ showAttris NOTIFY showAttrisChanged)
    Q_PROPERTY(bool editMode READ isEditMode NOTIFY editModeChanged)
    Q_PROPERTY(QColor valueColor READ valueColor WRITE setValueColor NOTIFY valueColorChanged)
public:
    enum EditAction {
        AddAction,
        InsertAboveAction,
        InsertBelowAction
    };
    Q_ENUM(EditAction)

    explicit JProtoTreeViewPrivate(JProtoTreeView *q);
    ~JProtoTreeViewPrivate() override;

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

    JProtoTreeView::ShowAttributes showAttributes() const;
    void setShowAttributes(JProtoTreeView::ShowAttributes attrs);
    void setShowAttribute(JProtoTreeView::ShowAttribute attr, bool on = true);
    bool testShowAttribute(JProtoTreeView::ShowAttribute attr) const;

    int dataFormat() const;
    void setDataFormat(int format);

    quint32 treeModes() const;
    void setTreeMode(JProtoTreeView::TreeModes modes);
    void setTreeMode(JProtoTreeView::TreeMode mode, bool on = true);
    bool testTreeMode(JProtoTreeView::TreeMode mode) const;

    bool isRunning() const;
    void setRunning(bool value);

    // batch process
    void bindingChannels(const QString &filePath);
    void unbindingChannels();
    void exportBindingStatus(const QString &filePath);
    void exportTableData(QStandardItem *item);
    void runAllChannels();
    void stopAllChannels();

    bool isEditMode() const;
    bool hasUnloadedItem(QStandardItem *item = nullptr) const;
    QStandardItem *unloadedItem(QStandardItem *item = nullptr) const;
    static bool isItemLoaded(QStandardItem *item);
    Icd::ObjectPtr findObject(QStandardItem *item, bool ignoreComplex = true) const;
    Icd::ObjectPtr findObject(const QString &domain, int domainType = Icd::DomainId,
                              bool ignoreComplex = true) const;

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
    void treeModesChanged();
    void showAttrisChanged();
    void editModeChanged();
    void valueColorChanged(const QColor &color);

    void itemUnloaded(QStandardItem *item, QStandardItem *itemTable);
    void channelBound(QStandardItem *item, const QString &channelId);
    void channelUnbound(QStandardItem *item, const QString &channelId);
    void channelChanged(QStandardItem *item, const QString &channelId);
    void unbindItem(QStandardItem *item, QStandardItem *itemTable);

public slots:
    void setValueColor(const QColor &color);

    void onTreeItemPressed(QStandardItem *item);
    void onCurrentItemChanged(QStandardItem *current, QStandardItem *previous);

    void onWorkerRemoved(const Icd::WorkerPtr &worker);
    void onWorkerCleared();

protected:
    QStringList mimeTypes() const override;
    QMimeData *mimeData(const QList<QStandardItem *> &items) const override;
    void keyPressEvent(QKeyEvent *event) override;

private:
    void itemRootRightClicked(QStandardItem *item, int deep);
    void itemVehicleRightClicked(QStandardItem *item, int deep);
    void itemSystemRightClicked(QStandardItem *item, int deep);
    void itemTableRightClicked(QStandardItem *item, int deep);
    void itemDataItemRightClicked(QStandardItem *item, int deep);
    void itemItemTableRightClicked(QStandardItem *item, int deep);
    void itemItemBitMapRightClicked(QStandardItem *item, int deep);

    void triggerCopyItem(QStandardItem *item);
    void triggerCloneItem(QStandardItem *item);

private:
    bool loadRoot(QStandardItem *itemRoot, int deep, bool force = false);
    bool loadVehicle(QStandardItem *itemVehicle, int deep, bool force = false);
    bool loadVehicle(QStandardItem *itemVehicle, const Icd::SystemPtrArray &systems, int deep);
    bool loadSystem(QStandardItem *itemSystem, int deep, bool force = false);
    bool loadSystem(QStandardItem *itemSystem, const Icd::TablePtrArray &tables, int deep);
    bool loadTable(QStandardItem *itemTable, int deep, bool force = false);

    QStandardItem *insertVehicle(int row, QStandardItem *itemRoot, const Icd::VehiclePtr &vehicle, int deep);
    QStandardItem *insertSystem(int row, QStandardItem *itemVehicle, const Icd::SystemPtr &system,
                                const QString &domain, const QString &path, int deep);
    QStandardItem *insertSystem(int row, QStandardItem *itemVehicle, const Icd::SystemPtr &system, int deep);
    QStandardItem *insertTable(int row, QStandardItem *itemParent, const Icd::TablePtr &table,
                               const QString &domain, const QString &path, int deep);
    QStandardItem *insertTable(int row, QStandardItem *itemParent, const Icd::TablePtr &table, int deep);

    static bool loadItems(QObject *target, QStandardItem *itemTable, const Icd::ItemPtrArray &items, int deep);

    static QStandardItem *loadIndexTable(QObject *target, QStandardItem *itemDataItem, const Icd::TablePtr &table, int index = 0);
    static QStandardItem *insertItem(QObject *target, int row, QStandardItem *itemTable, const Icd::ItemPtr &item);
    static bool loadFrameCodeItem(QObject *target, QStandardItem *itemTable, const Icd::FrameCodeItemPtr &frameCodeItem);
    static bool loadComplexItem(QObject *target, QStandardItem *itemDataItem, const Icd::ComplexItemPtr &complexItem);
    static bool loadFrameItem(QObject *target, QStandardItem *itemDataItem, const Icd::FrameItemPtr &frameItem);
    static bool loadBitItem(QObject *target, QStandardItem *itemDataItem, const Icd::BitItemPtr &bitItem);

    //
    void removeTableItem(QStandardItem *item);

    Icd::WorkerPtr queryWorker(const QStandardItem *itemTable) const;

    void updateVehicleData(QStandardItem *item, const Icd::VehiclePtr &vehicle);
    void updateSystemData(QStandardItem *item, const Icd::SystemPtr &system, const QString &path);
    void updateSystemData(QStandardItem *item, const Icd::SystemPtr &system);
    void updateTableData(QStandardItem *item, const Icd::TablePtr &table, const QString &path);
    void updateTableData(QStandardItem *item, const Icd::TablePtr &table);
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
    JWorkerGroup *findWorkerGroup(QStandardItem *itemTable) const;

    bool hasItemBound(QStandardItem *item);
    void clearItemBoundRole(QStandardItem *item, bool bEmit);

    void clearItemChildren(QStandardItem *item);

    void setItemLoadStatus(QStandardItem *item, bool loaded);

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
    Icd::ObjectPtr findByDomain(const QString &domain) const;
    template<typename T> inline std::shared_ptr<T> findByDomain(const QString &domain) const;

    ///

    static QString idDomain(QStandardItem *item);
    static QString markDomain(QStandardItem *item);
    static bool loadDeepTable(JTreeView *treeView, QStandardItem *itemParent, const Icd::TablePtr &table, int deep);

    //
    QMenu *createAddItemMenu(QStandardItem *item, QAction *action, EditAction editAction);

    void insertRow(int row, const Icd::ObjectPtr &target, const QVariant &data = QVariant());
    void updateRow(int sourceRow, int targetRow, const Icd::ObjectPtr &target, const QVariant &data = QVariant());
    void removeRow(int row, const Icd::ObjectPtr &target, const QVariant &data = QVariant());

    void insertNewBeside(QStandardItem *item, int editAction, const QVariant &data = QVariant());
    void applyInsert(const Icd::ObjectPtr &target);
    void cancelInsert();
    void removeRows(QStandardItem *parentItem, const QList<int> &rows);
    void removeRows(QStandardItem *parentItem, int beginRow, int endRow);
    void removeRow(QStandardItem *item, bool clearSelection = true);
    void clearItem(QStandardItem *parentItem);

    void updateOffset(QStandardItem *item, Icd::Object *object, int startRow);
    void updateOffset(QStandardItem *item, Icd::Object *object);
    void updateOffset(QStandardItem *item, Table *table, int startRow = 0);
    void updateOffset(QStandardItem *item, Icd::Item *dataItem);
    void updateOffset(QStandardItem *item, Icd::FrameItem *frame);
    void updateOffset(QStandardItem *item, Icd::ComplexItem *complex, int startRow = 0);

private:
    J_DECLARE_PUBLIC(JProtoTreeView)
    JSearchEdit *searchEdit_;
    Icd::ParserPtr parser_;
    Icd::RootPtr protoRoot_;
    int loadingDeep_;
    int intervalUpdate_;
    JProtoTreeView::BindTableTypes bindTableTypes_;
    JProtoTreeView::ShowAttributes showAttris_;
    int dataFormat_;
    JProtoTreeView::TreeModes treeModes_;
    QHash<QStandardItem * /*itemTable*/, JWorkerGroup*> workerGroups_;
    QFutureWatcher<BindingData> watcher_;
    // qss
    QColor valueColor_;
    // for edit module
    QStandardItem *newItem_;
    Icd::ObjectPtr newObject_;
};

//
template<typename T>
std::shared_ptr<T> JProtoTreeViewPrivate::findByDomain(const QString &domain) const
{
    Icd::ObjectPtr object = findByDomain(domain);
    if (object) {
        return JHandlePtrCast<T>(object);
    } else {
        return std::shared_ptr<T>();
    }
}

//
struct BindingData
{
    QString channelId;
    QString tableDomain;
    JProtoTreeViewPrivate *d;
    QStandardItem *item;
    Icd::WorkerPtr worker;
    Icd::TablePtr table;

    BindingData() : d(nullptr) {}
    BindingData(const QString &channelId, const QString &tableDomain,
                JProtoTreeViewPrivate *d)
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

} // end of namespace Icd

Q_DECLARE_METATYPE(Icd::BindingData)

#endif // JPROTOTREEVIEW_P_H
