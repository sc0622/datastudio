#ifndef DETAILTABLE_H
#define DETAILTABLE_H

#include <QWidget>
#include "main_global.h"
#include <QStyledItemDelegate>
#include "icdcore/icdcore_global.h"

class JTableView;

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
}

class QLabel;

namespace Edit {

class ViewDelegate;

class DetailTable : public QWidget
{
    Q_OBJECT
public:
    explicit DetailTable(QWidget *parent = nullptr);

    void resetView();
    void updateView(const Icd::ObjectPtr &object);

    Icd::ObjectPtr object() const;

    void setTip(const QString &text) const;

    bool isEditing() const;
    void setEditing(bool editing);

    int rowCount() const;
    int currentRow() const;
    Icd::icd_uint64 currentIndex() const;
    void updateRow(int row);

    void insertRow(int row, const Icd::VehiclePtr &vehicle);
    void insertRow(int row, const Icd::SystemPtr &system);
    void insertRow(int row, const Icd::TablePtr &table);
    void applyInsert();
    void cancelInsert();

signals:
    void currentItemChanged(const QVariant &index, const Icd::ObjectPtr &newObject);

public slots:
    void showContextMenu(const QPoint &pos);

private:
    bool updateRoot();
    bool updateVehicle();
    bool updateSystem();
    bool updateTable();
    bool updateItem();
    bool updateHeader();
    bool updateCounter();
    bool updateCheck();
    bool updateFrameCode();
    bool updateNumeric();
    bool updateArray();
    bool updateBit();
    bool updateBitMap();
    bool updateBitValue();
    bool updateComplex();
    bool updateFrame();

    void setRowData(int row, const Icd::VehiclePtr &vehicle);
    void setRowData(int row, const Icd::SystemPtr &system);
    void setRowData(int row, const Icd::TablePtr &table);
    void setRowData(int row, const Icd::ItemPtr &item, double offset);

    int insertRow(int row, const Icd::ObjectPtr &object);

private:
    JTableView *tableView_;
    QList<QAction*> actions_;
    QLabel *labelTip_;
    ViewDelegate *delegate_;
    Icd::ObjectPtr object_;
    Icd::ObjectPtr newObject_;
    bool editing_;
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
