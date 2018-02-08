#ifndef DETAILTABLE_H
#define DETAILTABLE_H

#include <QWidget>
#include "main_global.h"

class JTableView;

namespace Icd {
class Object;
typedef JHandlePtr<Object> ObjectPtr;
}

namespace Edit {

class DetailTable : public QWidget
{
    Q_OBJECT
public:
    explicit DetailTable(QWidget *parent = nullptr);

    void resetView();
    void updateView(const Icd::ObjectPtr &object);

signals:

public slots:

private:
    void updateObject();
    void updateRoot();
    void updateVehicle();
    void updateSystem();
    void updateTable();
    void updateItem();
    void updateHead();
    void updateCounter();
    void updateCheck();
    void updateFrameCode();
    void updateNumeric();
    void updateBitMap();
    void updateBitValue();
    void updateComplex();
    void updateFrame();

private:
    JTableView *d_tableView;
    Icd::ObjectPtr d_object;
};

}

#endif // DETAILTABLE_H
