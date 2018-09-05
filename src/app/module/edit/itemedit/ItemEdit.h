#ifndef ITEM_EDIT_H
#define ITEM_EDIT_H

#include "ObjectEdit.h"

namespace Icd {
class Item;
typedef std::shared_ptr<Item> ItemPtr;
}

namespace Edit {

// class ItemEdit

class ItemEdit : public ObjectEdit
{
    Q_OBJECT
public:
    explicit ItemEdit(const Icd::ItemPtr &item, QWidget *parent = nullptr);
    virtual ~ItemEdit();

    static ItemEdit *create(const Icd::ItemPtr &item);

    Icd::ItemPtr item() const;

signals:

public slots:

private:
};

}

#endif // ITEM_EDIT_H
