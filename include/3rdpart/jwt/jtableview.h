#ifndef JTABLEVIEW_H
#define JTABLEVIEW_H

#include "jwt_global.h"
#include <QTableView>
#include <QStandardItemModel>
#include <QSortFilterProxyModel>
#include <QStyledItemDelegate>

// - class JTableItemDelegate -

class JWT_EXPORT JTableItemDelegate : public QStyledItemDelegate
{
    Q_OBJECT
public:
    explicit JTableItemDelegate(QObject *parent = Q_NULLPTR);
    virtual ~JTableItemDelegate();

    static QString simplified(const QString &text);

    // painting
    void paint(QPainter *painter,
               const QStyleOptionViewItem &option, const QModelIndex &index) const Q_DECL_OVERRIDE;
};

// - class JTableSortFilterModel -

class JTableSortFilterModelData;

class JWT_EXPORT JTableSortFilterModel : public QSortFilterProxyModel
{
    Q_OBJECT
public:
    explicit JTableSortFilterModel(QObject *parent = 0);
    ~JTableSortFilterModel();

    QRegExp filterRegExp(int column) const;
    void setFilterRegExp(int column, const QRegExp &regExp);
    void removeFilter(int column);
    void clearFilter();

protected:
    virtual QString itemText(const QModelIndex &index) const;
    virtual bool filterAcceptsRow(int source_row, const QModelIndex &source_parent) const;

private:
    JTableSortFilterModelData *d;
};

// - class JTableViewSelectionRange -

class JWT_EXPORT JTableViewSelectionRange
{
public:
    JTableViewSelectionRange();
    JTableViewSelectionRange(int top, int left, int bottom, int right);
    JTableViewSelectionRange(const JTableViewSelectionRange &other);
    ~JTableViewSelectionRange();

    inline int topRow() const { return top; }
    inline int bottomRow() const { return bottom; }
    inline int leftColumn() const { return left; }
    inline int rightColumn() const { return right; }
    inline int rowCount() const { return bottom - top + 1; }
    inline int columnCount() const { return right - left + 1; }

    JTableViewSelectionRange &operator =(const JTableViewSelectionRange &other);

private:
    int top, left, bottom, right;
};

// - class JTableViewItem -

class JTableView;

class JWT_EXPORT JTableViewItem : public QStandardItem
{
    friend class JTableView;
public:
    explicit JTableViewItem(int type = Type);
    explicit JTableViewItem(const QString &text, int type = Type);
    explicit JTableViewItem(const QIcon &icon, const QString &text, int type = Type);
    explicit JTableViewItem(int rows, int columns, int type = Type);
    virtual ~JTableViewItem();

    virtual QStandardItem *clone() const;

    inline JTableView *view() const;

    void setSelected(bool select);
    bool isSelected() const;

    void setHidden(bool hide);
    bool isHidden() const;

    void setExpanded(bool expand);
    bool isExpanded() const;

    void setFirstColumnSpanned(bool span);
    bool isFirstColumnSpanned() const;

    void setDisabled(bool disabled);
    bool isDisabled() const;

    enum ChildIndicatorPolicy { ShowIndicator, DontShowIndicator, DOntShowIndicatorWhenChildless };
    void setChildIndicatorPolicy(JTableViewItem::ChildIndicatorPolicy policy);
    JTableViewItem::ChildIndicatorPolicy childIndicatorPolicy() const;

#ifndef QT_NO_DATASTREAM
    virtual void read(QDataStream &in);
    virtual void write(QDataStream &out) const;
#endif
    virtual bool operator<(const QStandardItem &other) const;

    inline int type() const { return _type; }

protected:
    JTableViewItem(const JTableViewItem &other);
    JTableViewItem &operator=(const JTableViewItem &other);

private:
    int _type;
    ChildIndicatorPolicy _policy;
};

// - class JTableView -

class JTableViewPrivate;

class JWT_EXPORT JTableView : public QTableView
{
    Q_OBJECT
    Q_PROPERTY(int rowCount READ rowCount WRITE setRowCount)
    Q_PROPERTY(int columnCount READ columnCount WRITE setColumnCount)
    friend class JHeaderArea;
public:
    explicit JTableView(QWidget *parent);
    virtual ~JTableView();

    int rowCount() const;
    void setRowCount(int rows);

    int columnCount() const;
    void setColumnCount(int columns);

    QStandardItem *horizontalHeaderItem(int column) const;
    QStandardItem *verticalHeaderItem(int row) const;

    void setVerticalHeaderLabels(const QStringList &labels);
    void setHorizontalHeaderLabels(const QStringList &labels);
    void setVerticalHeaderVisible(bool visible);

    QStandardItem *item(int row, int column) const;
    void setItem(int row, int column, QStandardItem *item);

    QVariant itemData(int row, int column, int role = Qt::DisplayRole) const;
    bool setItemData(int row, int column, const QVariant &value, int role = Qt::EditRole);

    bool insertRow(int row, QStandardItem *item);
    bool insertRow(int row, const QList<QStandardItem *> &items);

    int currentRow() const;
    int currentColumn() const;
    QStandardItem *currentItem() const;
    void setCurrentItem(QStandardItem *item);
    void setCurrentCell(int row, int column);

    QWidget *cellWidget(int row, int column) const;
    void setCellWidget(int row, int column, QWidget *widget);
    inline void removeCellWidget(int row, int column);

    bool isItemSelected(const QStandardItem *item) const;
    void setItemSelected(const QStandardItem *item, bool select);
    void setRangeSelected(const JTableViewSelectionRange &range, bool select);

    QStandardItem *itemAt(const QPoint &p) const;
    inline QStandardItem *itemAt(int x, int y) const;
    QRect visualItemRect(const QStandardItem *item) const;

    QList<JTableViewSelectionRange> selectRanges() const;

    QStandardItemModel *sourceModel() const;

Q_SIGNALS:
    void itemPressed(QStandardItem *item);
    void itemClicked(QStandardItem *item);
    void itemDoubleClicked(QStandardItem *item);

    void itemActivated(QStandardItem *item);
    void itemEntered(QStandardItem *item);
    void itemChanged(QStandardItem *item);

    void currentItemChanged(QStandardItem *current, QStandardItem *previous);
    void itemSelectionChanged();

    void cellPressed(int row, int column);
    void cellClicked(int row, int column);
    void cellDoubleClicked(int row, int column);

    void cellActivated(int row, int column);
    void cellEntered(int row, int column);
    void cellChanged(int row, int column);

    void currentCellChanged(int currentRow, int currentColumn, int previousRow, int previuosColumn);

public Q_SLOTS:
    void scrollToItem(const QStandardItem *item, QAbstractItemView::ScrollHint hint = EnsureVisible);
    void scrollToRow(int row, QAbstractItemView::ScrollHint hint = EnsureVisible);
    void scrollToColumn(int column, QAbstractItemView::ScrollHint hint = EnsureVisible);
    void insertRow(int row);
    void insertRow(int row, int count);
    void insertColumn(int column);
    void insertColumn(int column, int count);
    void removeRow(int row);
    void removeRow(int row, int count);
    void removeColumn(int column);
    void removeColumn(int column, int count);
    void clear();
    void clearContents();

protected Q_SLOTS:
    virtual void selectionChanged(const QItemSelection &selected, const QItemSelection &deselected);
    virtual void currentChanged(const QModelIndex &current, const QModelIndex &previous);

protected:
    virtual QStringList mimeTypes() const;
    virtual QMimeData *mimeData(const QList<QStandardItem *> &items) const;
    virtual bool dropMimeData(QStandardItem *parent, int row, int column,
                              const QMimeData *data, Qt::DropAction action);
    virtual Qt::DropActions supportedDropActions() const;
    QList<QStandardItem *> items(const QMimeData *data) const;

    QModelIndex indexFromItem(QStandardItem *item) const;
    QStandardItem *itemFromIndex(const QModelIndex &index) const;

private:
    void setModel(QAbstractItemModel *model);

public:
    JTableViewPrivate *privateData();

private:
    J_DECLARE_PRIVATE(JTableView)
    friend class JTableModel;
};

inline void JTableView::removeCellWidget(int row, int column)
{ setCellWidget(row, column, 0); }

inline QStandardItem *JTableView::itemAt(int x, int y) const
{ return itemAt(QPoint(x, y)); }

#endif // JTABLEVIEW_H
