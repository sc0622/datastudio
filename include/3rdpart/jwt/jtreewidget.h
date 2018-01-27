#ifndef JTREEWIDGET_H
#define JTREEWIDGET_H

#include "jwt_global.h"
#include <QTreeWidget>
#include <QMimeData>
#include <QVariant>
#include <list>
#include <map>
#include <string>

// - struct JTreeItemData -

struct JTreeItem;

struct JTreeItemData
{
    QString identity;
    int type;
    bool bDrag;             // drag enabled
    std::string extData;    //

    JTreeItem *item;

    JTreeItemData()
    {
        reset();
    }

    JTreeItemData(const JTreeItemData &other)
    {
        *this = other;
    }

    JTreeItemData &operator =(const JTreeItemData &other)
    {
        if (this == &other) {
            return *this;
        }

        identity = other.identity;
        type = other.type;
        bDrag = other.bDrag;
        extData = other.extData;
        item = other.item;

        return *this;
    }

    void reset()
    {
        identity.clear();
        type = -1;
        bDrag = false;
        extData.clear();
        item = 0;
    }
};

// - struct JTreeItemWidget -

struct JTreeItemWidget
{
    int column;
    QWidget *widget;

    JTreeItemWidget() :
        column(-1),
        widget(0)
    {

    }

    JTreeItemWidget(int column, QWidget *widget) :
        column(column),
        widget(widget)
    {

    }

    JTreeItemWidget(const JTreeItemWidget &other)
    {
        *this = other;
    }

    JTreeItemWidget &operator =(const JTreeItemWidget &other)
    {
        if (this == &other) {
            return *this;
        }

        column = other.column;
        widget = other.widget;

        return *this;
    }

    void reset()
    {
        column = -1;
        widget = 0;
    }
};

// - struct JTreeItem -

struct JTreeItem
{
    std::string text;
    std::string icon;
    std::string number;
    bool checkable;
    bool drag;          // drag enabled
    int checkState;     // 0:Unchecked, 1:PartiallyChecked, 2:Checked
    void *data;
    std::map<int/*column*/, JTreeItemWidget> widgets;

    // for table
    int columnOffset;   //
    int columnCount;    // items中最大的列数

    JTreeItemData itemData;
    std::list<JTreeItem> items;
    JTreeItem *parent;

    JTreeItem()
    {
        reset();
    }

    JTreeItem *findParent(const std::string &number)
    {
        JTreeItem *current = this;
        while (true) {
            if (current->number == number || current->parent == 0) {
                return current;
            }

            current = current->parent;
        }

        return 0;
    }

    JTreeItem *findItem(const std::string &identity)
    {
        if (itemData.identity == QString::fromStdString(identity)) {
            return this;
        }

        std::list<JTreeItem>::iterator iterItems = items.begin();
        for (; iterItems != items.end(); ++iterItems) {
            JTreeItem *current = iterItems->findItem(identity);
            if (current != 0) {
                return current;
            }
        }

        // 如果找不到节点，就挂载根节点上
        return parent == 0 ? this : 0;
    }

    // 复位数据
    void reset()
    {
        text.clear();
        icon.clear();
        number.clear();
        checkable = false;
        checkState = Qt::Unchecked;
        data = 0;
        widgets.clear();

        drag = false;

        columnOffset = 0;
        columnCount = 0;

        itemData.reset();
        items.clear();
        parent = 0;
    }

    // 更新节点的表格列信息
    int updateColumn()
    {
        if (items.empty()) {
            return 1;
        }

        columnCount = 0;

        std::list<JTreeItem>::iterator iterItems = items.begin();
        for (; iterItems != items.end(); ++iterItems) {
            JTreeItem &item = *iterItems;
            item.columnOffset = columnOffset + 1;
            int count = item.updateColumn();
            if (count > columnCount) {
                columnCount = count;
            }
        }

        return columnCount + 1;
    }
};

// - class JTreeWidgetUserData -

template<typename T>
class JTreeWidgetUserData : public QObjectUserData
{
public:
    JTreeWidgetUserData() {}
    ~JTreeWidgetUserData() {}

    T &treeItem()
    {
        return d;
    }

private:
    T d;
};

// - class JCellWidgetData -

class JCellWidgetData : public QObjectUserData
{
public:
    JCellWidgetData() {}
    ~JCellWidgetData() {}

    QVariant data() const { return _v; }
    void setData(const QVariant &v) { _v = v; }
    bool isValid() const { return _v.isValid(); }
    bool isNull() const { return _v.isNull(); }

    static JCellWidgetData *userData(QObjectUserData *d)
    {
        return dynamic_cast<JCellWidgetData *>(d);
    }

    static QVariant data(QObjectUserData *d)
    {
        JCellWidgetData *_d = userData(d);
        return _d ? _d->data() : QVariant();
    }

private:
    QVariant _v;
};

// - class JTreeItemMimeData -

class JTreeItemMimeData : public QMimeData
{
    Q_OBJECT
public:
    JTreeItemMimeData() :
        q_dragItem(0)
    {

    }

    virtual ~JTreeItemMimeData()
    {

    }

    void setDragData(const QString &mimeType, QTreeWidgetItem *item)
    {
        q_format.append(mimeType);
        q_dragItem = item;
    }

    QStringList format() const { return q_format; }
    const QTreeWidgetItem *dragItem() const { return q_dragItem; }

protected:
    QVariant retrieveData(const QString &mimetype, QVariant::Type preferredType) const
    {
        return (mimetype == "ItemMimeData") ?
                    q_dragItem : QMimeData::retrieveData(mimetype, preferredType);
    }

private:
    const QTreeWidgetItem *q_dragItem;
    QStringList q_format;
};

// - class JTreeWidget -

class JWT_EXPORT JTreeWidget : public QTreeWidget
{
    Q_OBJECT
public:
    // 启用复选框，初始化必须设置此属性，否则将导致复选框状态不正确
    // e.g. setData(column, ItemCheckableState, Qt::Checked);
    enum { ItemCheckableState = Qt::UserRole + 100 };

    explicit JTreeWidget(QWidget *parent = 0);
    ~JTreeWidget();

    void enableTooltip(bool enabled = true);

    // 加载节点，如果node为调用者new的，调用者须自己负责释放
    bool loadTreeItems(QTreeWidgetItem *parentItem);
    bool loadTreeItems(QTreeWidgetItem *parentItem, const JTreeItem &item);
    bool loadTreeItems(QTreeWidgetItem *parentItem, const std::list<JTreeItem> &items);

    // 加载树表
    bool loadTableItems(QTreeWidgetItem *parentItem, const JTreeItem &item);
    bool loadTableItems(QTreeWidgetItem *parentItem, const std::list<JTreeItem> &items);

    // 如果node为调用者new的，调用者须自己负责释放；如果此为父节点，同时删除其子节点
    void deleteItem(const QString &identity);

    //
    QTreeWidgetItem *rootItem();
    void setRootItem(QTreeWidgetItem *item);

    // 根据节点唯一标识查找已插入的节点，没有找到则返回0
    QTreeWidgetItem *findTreeItem(const QString &identity, QTreeWidgetItem *current = 0);

    // 根据节点获取唯一标识
    QString identityOfTreeItem(QTreeWidgetItem *item) const;

    // 获取附加在节点上的信息，返回false表示获取失败
    static bool treeItemData(const QTreeWidgetItem *item, JTreeItemData &itemData);
    static JTreeItemData *treeItemData(const QTreeWidgetItem *item);

    // 获取当前节点的数据信息
    JTreeItemData *currentTreeItemData();

    // 获取当前节点数据
    JTreeItem *currentTreeItem();

    // 使能/禁用onSlotItemChanged链接
    void enableSlotItemChanged(bool enable = true);

    // 使能？禁用节点复选框功能
    void enableAllCheckable(bool enable = true);

    // 使能/禁用节点图标功能
    void setDefaultIconEnable(bool enable = true);

    // 获取节点数据
    template<typename T> T &jTreeItem();
    template<typename RetType, typename RoleType>
    RetType jTreeItem(QTreeWidgetItem *item, int column = 0, int role = Qt::UserRole) const;

    // 获取节点数据
    template<typename T> JTreeWidgetUserData<T> *jUserData();

    // 设置节点数据（bSelf: true，由自己释放；false，不自己释放（用于绑定））
    template<typename T> void jSetUserData(JTreeWidgetUserData<T> *userData, bool bSelf = true);

    // 展开单个节点及其子节点
    void setItemAllExpandded(QTreeWidgetItem *item, bool expand = true);

    // 设置节点及其子节点复选框状态
    void setItemState(QTreeWidgetItem *item, int column, Qt::CheckState state);

    // 设置所有节点复选框状态
    void setAllItmeState(Qt::CheckState state, QTreeWidgetItem *item = 0);

    // 拖拽状态
    bool dragState() const { return q_isDrag; }
    void setDragState(bool state) { q_isDrag = state; }

public Q_SLOTS:
    void slotItemChanged(QTreeWidgetItem *item, int column);
    void slotItemCheckedChanged(int state);

protected:
    //
    int uniqueIndex();

    // 设置子节点复选框状态
    void setChildrenState(QTreeWidgetItem *item, int column, Qt::CheckState state);

    // 设置兄弟节点复选框状态
    void setSiblingState(QTreeWidgetItem *item, int column, Qt::CheckState state);

    // 设置父节点复选框状态
    void setParentState(QTreeWidgetItem *item, int column, Qt::CheckState state);

    // 检查一节点及其子节点是否全部未启用复选框
    bool partCheckedDisabled(QTreeWidgetItem *item) const;

    void paintEvent(QPaintEvent *event);
    void mouseMoveEvent(QMouseEvent *event);
    void mousePressEvent(QMouseEvent *event);
    void mouseReleaseEvent(QMouseEvent *event);
    void dragEnterEvent(QDragEnterEvent *event);
    void dragMoveEvent(QDragMoveEvent *event);
    void dropEvent(QDropEvent *event);

    virtual void performDrag();

private:
    // 递归设置所有节点状态
    void setItemState(QTreeWidgetItem *item, Qt::CheckState state);

private:
    bool q_checkable;
    bool q_enableIcon;
    bool q_enableTooltip;
    bool q_enableConnection;        // 消息启用标识
    bool q_isDrag;                  // 拖拽标识
    QTreeWidgetItem *q_rootItem;    //
    QPoint q_startDragPoint;        // 拖拽起始点坐标

    //
    struct JTreeWidgetData
    {
        QObjectUserData *userData;
        bool bSelf; //

        JTreeWidgetData() : userData(0), bSelf(true) {}
    } q_userData;
};

// 获取节点树数据
template<typename T>
inline T &JTreeWidget::jTreeItem()
{
    return jUserData<T>()->treeItem();
}

// 获取节点树数据
template<typename RetType, typename RoleType>
inline RetType JTreeWidget::jTreeItem(QTreeWidgetItem *item, int column, int role) const
{
    if (!item) {
        return 0;
    }

    QVariant v = item->data(column, role);
    if (v.isNull()) {
        return 0;
    }

    return reinterpret_cast<RetType>(v.value<RoleType>());
}

// 获取用户节点数据
template<typename T>
inline JTreeWidgetUserData<T> *JTreeWidget::jUserData()
{
    return reinterpret_cast<JTreeWidgetUserData<T> *>(q_userData.userData);
}

// 设置节点数据（bSelf: true，由自己释放；false，不自己释放（用于绑定））
template<typename T>
inline void JTreeWidget::jSetUserData(JTreeWidgetUserData<T> *userData, bool bSelf)
{
    if (q_userData.userData != 0) {
        if (q_userData.userData == userData) {
            return;
        }

        if (q_userData.bSelf) {
            delete q_userData.userData;
        }
    }

    q_userData.userData = userData;
    q_userData.bSelf = bSelf;
}

#endif // JTREEWIDGET_H
