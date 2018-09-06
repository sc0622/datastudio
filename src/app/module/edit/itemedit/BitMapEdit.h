#ifndef BITMAP_EDIT_H
#define BITMAP_EDIT_H

#include "BitEdit.h"

class JLargeSpinBox;

namespace Edit {

// class BitMapEdit

class BitMapEdit : public BitEdit
{
    Q_OBJECT
public:
    explicit BitMapEdit(const Icd::BitItemPtr &bit, QWidget *parent = nullptr);
    ~BitMapEdit();

    bool init() override;

signals:

public slots:

private:
    JLargeSpinBox *spinDefaultValue_;
};

}

#endif // BITMAP_EDIT_H
