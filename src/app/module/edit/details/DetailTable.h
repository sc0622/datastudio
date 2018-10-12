#ifndef DETAILTABLE_H
#define DETAILTABLE_H

#include <QWidget>
#include "main_global.h"
#include <QStyledItemDelegate>
#include "icdcore/icdcore_global.h"
#include "jwt/jtableview.h"

class JTableView;
class QStandardItem;

namespace Icd {
class Object;
typedef std::shared_ptr<Object> ObjectPtr;
class Vehicle;
typedef std::shared_ptr<Vehicle> VehiclePtr;
class System;
typedef std::shared_ptr<System> SystemPtr;
class Table;
typedef std::shared_ptr<Table> TablePtr;
class Item;
typedef std::shared_ptr<Item> ItemPtr;
class HeaderItem;
typedef std::shared_ptr<HeaderItem> HeaderItemPtr;
class CounterItem;
typedef std::shared_ptr<CounterItem> CounterItemPtr;
class CheckItem;
typedef std::shared_ptr<CheckItem> CheckItemPtr;
class FrameCodeItem;
typedef std::shared_ptr<FrameCodeItem> FrameCodeItemPtr;
class NumericItem;
typedef std::shared_ptr<NumericItem> NumericItemPtr;
class ArrayItem;
typedef std::shared_ptr<ArrayItem> ArrayItemPtr;
class BitItem;
typedef std::shared_ptr<BitItem> BitItemPtr;
class ComplexItem;
typedef std::shared_ptr<ComplexItem> ComplexItemPtr;
class FrameItem;
typedef std::shared_ptr<FrameItem> FrameItemPtr;
}

class QLabel;

namespace Edit {

// class DetailTableView

class DetailTableView : public JTableView
{
    Q_OBJECT
public:
    explicit DetailTableView(QWidget *parent = nullptr);

    void enableSelection(bool enabled);
    void enableDragAndDrop(bool enabled);

    bool isMultiRowSelected() const;

signals:
    void movingDropped(int sourceRow, int targetRow);

protected:
    void keyPressEvent(QKeyEvent *event) override;
    void keyReleaseEvent(QKeyEvent *event) override;
    void mousePressEvent(QMouseEvent *event) override;
    void dropEvent(QDropEvent *event) override;

private:
    QAbstractItemView::SelectionMode selectionMode_;
};

// class DetailTable

class ViewDelegate;

class DetailTable : public QWidget
{
    Q_OBJECT
public:
    enum EditAction {
        AddAction,
        InsertAboveAction,
        InsertBelowAction
    };
    Q_ENUM(EditAction)

    explicit DetailTable(QWidget *parent = nullptr);

    void resetView();
    void updateView(const Icd::ObjectPtr &object);

    Icd::ObjectPtr object() const;

    void setTip(const QString &text) const;

    bool isModified() const;
    bool isMoving() const;
    bool isMoved() const;

    int rowCount() const;
    int currentRow() const;
    Icd::icd_uint64 currentIndex() const;
    int originalRow() const;
    bool isMultiRowSelected() const;
    void clearSelection();
    void updateRow(int row, const QVariant &data = QVariant::Invalid);

    void insertRow(int row, const Icd::VehiclePtr &vehicle);
    void insertRow(int row, const Icd::SystemPtr &system);
    void insertRow(int row, const Icd::TablePtr &table);
    void insertRow(int row, const Icd::ItemPtr &item);
    void removeRow(int row);
    void cleanItem();
    void moveCurrentRow(bool up);
    bool apply(const Icd::ObjectPtr &target, int row);
    void cancel();

    bool isSameType(const Icd::ObjectPtr &object) const;

signals:
    void currentItemChanged(const QVariant &index, const Icd::ObjectPtr &newObject);
    void rowMoved(int previousRow, int currentRow, bool restore);
    void requestInsert(int row, const QVariant &data);
    void requestPast(int row, const Icd::ObjectPtr &object, bool clone);

public slots:
    void showContextMenu(const QPoint &pos);

private:
    void setContextMenuEnabled(bool enabled);

    bool updateRoot();
    bool updateVehicle();
    bool updateSystem();
    bool updateTable();
    bool updateItem();

    bool updateTable(const Icd::TablePtr &table);
    bool updateItem(const Icd::ItemPtr &item);
    bool updateHeader(const Icd::HeaderItemPtr &header);
    bool updateCounter(const Icd::CounterItemPtr &counter);
    bool updateCheck(const Icd::CheckItemPtr &check);
    bool updateFrameCode(const Icd::FrameCodeItemPtr &frameCode);
    bool updateNumeric(const Icd::NumericItemPtr &numeric);
    bool updateArray(const Icd::ArrayItemPtr &array);
    bool updateBit(const Icd::BitItemPtr &bit);
    bool updateBitMap(const Icd::BitItemPtr &bit);
    bool updateBitValue(const Icd::BitItemPtr &bit);
    bool updateComplex(const Icd::ComplexItemPtr &complex);
    bool updateFrame(const Icd::FrameItemPtr &frame);

    void setRowData(int row, const Icd::VehiclePtr &vehicle);
    void setRowData(int row, const Icd::SystemPtr &system);
    void setRowData(int row, const Icd::TablePtr &table);
    void setRowData(int row, const Icd::ItemPtr &item, double offset);
    void setRowOffsetAndSize(int row, const Icd::ItemPtr &item, double offset);

    int insertRow(int row, const Icd::ObjectPtr &object);

    void moveBegin();
    void moveEnd();
    void moveRow(int sourceRow, int targetRow);

    void updateOffsetAndSize();
    void updateOffsetAndSize(const Icd::TablePtr &table);

    bool calcVerticalRange(const QList<JTableViewSelectionRange> &ranges, int &topRow, int &bottomRow);
    QMenu *createAddItemMenu(int row, QAction *action, int editAction);

private:
    DetailTableView *tableView_;
    QList<QAction*> actions_;
    QLabel *labelTip_;
    ViewDelegate *delegate_;
    Icd::ObjectPtr object_;
    Icd::ObjectPtr newObject_;
    bool moving_;
    int originalRow_;
};

//// private

// class NameItemDelegate

class NameItemDelegate : public QStyledItemDelegate
{
    Q_OBJECT
public:
    explicit NameItemDelegate(QObject *parent = nullptr);
};

}

#endif // DETAILTABLE_H
