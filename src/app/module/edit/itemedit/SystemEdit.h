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
    ~SystemEdit() override;

    Icd::SystemPtr system() const;

    bool init() override;

signals:

public slots:

protected:
    void restoreContent(bool recursive = true) override;
    bool validate() const override;
    void saveContent() override;

private:
};

}

#endif // VEHICLE_EDIT_H
