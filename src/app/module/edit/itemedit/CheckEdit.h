#ifndef CHECK_EDIT_H
#define CHECK_EDIT_H

#include "ItemEdit.h"

namespace Icd {
class CheckItem;
typedef std::shared_ptr<CheckItem> CheckItemPtr;
}

namespace Edit {

// class CheckEdit

class CheckEdit : public ItemEdit
{
    Q_OBJECT
public:
    explicit CheckEdit(const Icd::CheckItemPtr &check, QWidget *parent = nullptr);
    ~CheckEdit();

    Icd::CheckItemPtr check() const;

signals:

public slots:

private:
};

}

#endif // CHECK_EDIT_H
