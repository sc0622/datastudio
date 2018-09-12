#ifndef JPROTOTREEVIEW_H
#define JPROTOTREEVIEW_H

#include <QWidget>
#include "icdwidget_global.h"

class QStandardItem;
class JTreeView;

namespace Icd {

//
enum OptionType {
    // base
    OptionName,
    OptionMark,
    OptionDesc,
    //
};

class Root;
typedef std::shared_ptr<Root> RootPtr;

class Table;
typedef std::shared_ptr<Table> TablePtr;

class Parser;
typedef std::shared_ptr<Parser> ParserPtr;

// class JProtoTreeView

class JProtoTreeViewPrivate;

class ICDWIDGET_EXPORT JProtoTreeView : public QWidget
{
    Q_OBJECT
    Q_PROPERTY(bool running READ isRunning WRITE setRunning NOTIFY runningChanged)
public:
    enum BindTableType {
        BindOnlySend = 0x01,
        BindOnlyRecv = 0x02,
        BindAllTable = BindOnlySend | BindOnlyRecv
    };
    Q_DECLARE_FLAGS(BindTableTypes, BindTableType)

    enum TreeMode {
        TreeModeEdit = 0x0001,          /**< . */
        TreeModeMonitor = 0x0002,       /**< . */
        TreeModeSimulator = 0x0004,     /**< . */
        TreeModeAnalyse = 0x0008,       /**< . */
        TreeModeTableSel = 0x0010       /**< . */
    };
    Q_DECLARE_FLAGS(TreeModes, TreeMode)

    enum ShowAttribute {
        ShowOffset = 0x0001,    /**< . */
        ShowType = 0x0002,      /**< . */
        ShowData = 0x0004,      /**< . */
        ShowValue = 0x0008,     /**< . */
        ShowSpec = 0x0010      /**< . */
    };
    Q_DECLARE_FLAGS(ShowAttributes, ShowAttribute)

    enum EditAction {
        AddAction,
        InsertAboveAction,
        InsertBelowAction,
        RemoveAction,
        CleanAction
    };
    Q_ENUM(EditAction)

    explicit JProtoTreeView(QWidget *parent = nullptr);
    virtual ~JProtoTreeView();

    BindTableTypes bindTableType() const;
    void setBindTableType(BindTableTypes value);

    ShowAttributes showAttributes() const;
    void setShowAttributes(ShowAttributes attrs);
    void setShowAttribute(ShowAttribute attr, bool on = true);
    bool testShowAttribute(ShowAttribute attr) const;

    int dataFormat() const;

    TreeModes treeModes() const;
    void setTreeMode(TreeModes modes);
    void setTreeMode(TreeMode mode, bool on = true);
    bool testTreeMode(TreeMode mode) const;

    Icd::ParserPtr parser() const;
    void setParser(const Icd::ParserPtr &parser);

    int loadingDeep() const;
    void setLoadingDeep(int deep);

    int intervalUpdate() const;
    void setIntervalUpdate(int interval);

    void clearContents();
    bool loadData();
    bool loadData(const Icd::TablePtr &table, const QString &domain = QString());
    bool loadData(const Icd::TablePtr &table,
                  const QString &filePath,
                  bool hasTimeFormat, int headerSize,
                  const QString &domain = QString());

    QStandardItem *currentItem() const;

    bool isRunning() const;

    void bindingChannels(const QString &filePath);
    void unbindingChannels();

    void exportBindingStatus(const QString &filePath);

    void runAllChannels();
    void stopAllChannels();

    QStandardItem *findItemTable(QStandardItem *item) const;
    void selectItem(const QString &domain, int domainType);

    QString itemDomain(QStandardItem *item, int domainType) const;
    static QString idDomain(QStandardItem *item);
    static QString markDomain(QStandardItem *item);

    static bool loadTable(JTreeView *treeView, QStandardItem *itemParent, const Icd::TablePtr &table);

    Icd::RootPtr protoRoot() const;
    void setProtoRoot(const Icd::RootPtr &root);

    bool isItemLoaded(QStandardItem *item) const;
    bool hasUnloadedItem() const;

signals:
    void itemPressed(QStandardItem *item);
    void itemClicked(QStandardItem *item);
    void itemDoubleClicked(QStandardItem *item);
    void currentItemChanged(QStandardItem *current, QStandardItem *previous);
    void itemUnloaded(QStandardItem *item, QStandardItem *itemTable);
    void channelBound(QStandardItem *item, const QString &channelId);
    void channelUnbound(QStandardItem *item, const QString &channelId);
    void channelChanged(QStandardItem *item, const QString &channelId);
    void unbindItem(QStandardItem *item, QStandardItem *itemTable);
    void settingAxisTriggled();
    void runningChanged(bool value);
    void exportAnalyseData(QStandardItem *item, const QString &filePath,
                           bool hasTimeFormat, int headerSize);

    // {{ for editor
    void editItemTriggered(QStandardItem *item, EditAction action, const QVariant &data = QVariant());
    // }}

public slots:
    void setRunning(bool value);
    void setDataFormat(int format);

private:
    Q_DISABLE_COPY(JProtoTreeView)
    J_DECLARE_PRIVATE(JProtoTreeView)
};

} // end of namespace Icd

#endif // JPROTOTREEVIEW_H
