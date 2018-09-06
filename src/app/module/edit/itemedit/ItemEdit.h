#ifndef ITEM_EDIT_H
#define ITEM_EDIT_H

#include "ObjectEdit.h"

namespace Icd {
class Item;
typedef std::shared_ptr<Item> ItemPtr;
}

class QComboBox;

namespace Edit {

// class ItemEdit

class ItemEdit : public ObjectEdit
{
    Q_OBJECT
public:
    explicit ItemEdit(const Icd::ItemPtr &item, QWidget *parent = nullptr);
    virtual ~ItemEdit();

    Icd::ItemPtr item() const;

    static ItemEdit *create(const Icd::ItemPtr &item);

    bool init() override;

signals:

public slots:

private:
    QComboBox *comboType_;
};

}

#endif // ITEM_EDIT_H
