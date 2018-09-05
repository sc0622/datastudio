#ifndef DETAILTABLE_H
#define DETAILTABLE_H

#include <QWidget>
#include "main_global.h"
#include <QStyledItemDelegate>

class JTableView;

namespace Icd {
class Object;
typedef std::shared_ptr<Object> ObjectPtr;
}

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

signals:
    void currentItemChanged(const QVariant &index);
    void contentChanged(const QVariant &index, const QString &name);

public slots:
    void onContentChanged(QStandardItem *item);

private:
    bool updateRoot();
    bool updateVehicle();
    bool updateSystem();
    bool updateTable();
    bool updateItem();
    bool updateHead();
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

    bool saveRoot(QStandardItem *item);
    bool saveVehicle(QStandardItem *item);
    bool saveSystem(QStandardItem *item);
    bool saveTable(QStandardItem *item);
    bool saveItem(QStandardItem *item);
    bool saveHead(QStandardItem *item);
    bool saveCounter(QStandardItem *item);
    bool saveCheck(QStandardItem *item);
    bool saveFrameCode(QStandardItem *item);
    bool saveNumeric(QStandardItem *item);
    bool saveArray(QStandardItem *item);
    bool saveBit(QStandardItem *item);
    bool saveBitMap(QStandardItem *item);
    bool saveBitValue(QStandardItem *item);
    bool saveComplex(QStandardItem *item);
    bool saveFrame(QStandardItem *item);

private:
    void enableChangedNotify(bool enabled);

private:
    JTableView *tableView_;
    ViewDelegate *delegate_;
    Icd::ObjectPtr object_;
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
