#ifndef DETAILITEMEDIT_H
#define DETAILITEMEDIT_H

#include <QWidget>
#include "main_global.h"

class QGroupBox;

namespace Icd {
class Object;
typedef JHandlePtr<Object> ObjectPtr;
}

namespace Edit {

class DetailItemEdit : public QWidget
{
    Q_OBJECT
public:
    explicit DetailItemEdit(QWidget *parent = nullptr);

    void resetView();
    void updateView(const Icd::ObjectPtr &object);

signals:

public slots:

private:
    QGroupBox *d_groupBox;
    Icd::ObjectPtr d_object;
};

}

#endif // DETAILITEMEDIT_H
