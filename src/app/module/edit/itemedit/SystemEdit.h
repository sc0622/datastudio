#ifndef SYSTEM_EDIT_H
#define SYSTEM_EDIT_H

#include "ObjectEdit.h"

namespace Icd {
class System;
typedef std::shared_ptr<System> SystemPtr;
}

namespace Edit {

// class SystemEdit

class SystemEdit : public ObjectEdit
{
    Q_OBJECT
public:
    explicit SystemEdit(const Icd::SystemPtr &system, QWidget *parent = nullptr);
    ~SystemEdit();

    Icd::SystemPtr system() const;

signals:

public slots:

private:
};

}

#endif // VEHICLE_EDIT_H
