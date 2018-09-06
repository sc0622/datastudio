#ifndef BIT_EDIT_H
#define BIT_EDIT_H

#include "ItemEdit.h"

namespace Icd {
class BitItem;
typedef std::shared_ptr<BitItem> BitItemPtr;
}

class QSpinBox;

namespace Edit {

class SpecsTable;

// class BitEdit

class BitEdit : public ItemEdit
{
    Q_OBJECT
public:
    explicit BitEdit(const Icd::BitItemPtr &bit, QWidget *parent = nullptr);
    virtual ~BitEdit();

    Icd::BitItemPtr bit() const;

    static BitEdit *create(const Icd::BitItemPtr &bit);

    virtual bool init() override;

signals:

public slots:

private:
    QSpinBox *spinBitStart_;
    QSpinBox *spinBitCount_;
    SpecsTable *tableSpecs_;
};

}

#endif // BIT_EDIT_H
