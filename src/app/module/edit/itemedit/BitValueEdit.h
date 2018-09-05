#ifndef BITVALUE_EDIT_H
#define BITVALUE_EDIT_H

#include "BitEdit.h"

namespace Edit {

// class BitValueEdit

class BitValueEdit : public BitEdit
{
    Q_OBJECT
public:
    explicit BitValueEdit(const Icd::BitItemPtr &bit, QWidget *parent = nullptr);
    ~BitValueEdit();

signals:

public slots:

private:
};

}

#endif // BITVALUE_EDIT_H
