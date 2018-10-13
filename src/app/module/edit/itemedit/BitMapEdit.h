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
    ~BitMapEdit() override;

    bool init() override;

signals:

public slots:

protected:
    void restoreContent(bool recursive = true) override;
    bool validate() const override;
    void saveContent() override;

private:
    JLargeSpinBox *spinDefaultValue_;
};

}

#endif // BITMAP_EDIT_H
