#ifndef BITMAP_EDIT_H
#define BITMAP_EDIT_H

#include "BitEdit.h"

namespace Edit {

// class BitMapEdit

class BitMapEdit : public BitEdit
{
    Q_OBJECT
public:
    explicit BitMapEdit(const Icd::BitItemPtr &bit, QWidget *parent = nullptr);
    ~BitMapEdit();

signals:

public slots:

private:
};

}

#endif // BITMAP_EDIT_H
