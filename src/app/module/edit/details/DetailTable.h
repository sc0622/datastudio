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

signals:
    void currentItemChanged(const QVariant &index);
    void contentChanged(const QVariant &index, const QString &name);

public slots:

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

private:
    JTableView *tableView_;
    QLabel *labelTip_;
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
