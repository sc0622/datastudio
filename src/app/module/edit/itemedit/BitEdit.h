#ifndef BIT_EDIT_H
#define BIT_EDIT_H

#include "ItemEdit.h"

namespace Icd {
class BitItem;
typedef std::shared_ptr<BitItem> BitItemPtr;
}

namespace Edit {

// class BitEdit

class BitEdit : public ItemEdit
{
    Q_OBJECT
public:
    explicit BitEdit(const Icd::BitItemPtr &bit, QWidget *parent = nullptr);
    virtual ~BitEdit();

    static BitEdit *create(const Icd::BitItemPtr &bit);

    Icd::BitItemPtr bit() const;

signals:

public slots:

private:
};

}

#endif // BIT_EDIT_H
