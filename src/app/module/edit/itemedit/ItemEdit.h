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
    virtual ~ItemEdit() override;

    Icd::ItemPtr item() const;
    int itemType() const override;

    static ItemEdit *create(const Icd::ItemPtr &item);

    bool init() override;

signals:

public slots:

protected:
    virtual void restoreContent(bool recursive = true) override;
    virtual bool validate() override;
    virtual void saveContent() override;

private:
    QComboBox *comboType_;
};

}

#endif // ITEM_EDIT_H
