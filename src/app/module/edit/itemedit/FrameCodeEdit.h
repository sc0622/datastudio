#ifndef FRAMECODE_EDIT_H
#define FRAMECODE_EDIT_H

#include "ItemEdit.h"

namespace Icd {
class FrameCodeItem;
typedef std::shared_ptr<FrameCodeItem> FrameCodeItemPtr;
}

namespace Edit {

// class FrameCodeEdit

class FrameCodeEdit : public ItemEdit
{
    Q_OBJECT
public:
    explicit FrameCodeEdit(const Icd::FrameCodeItemPtr &frameCode, QWidget *parent = nullptr);
    ~FrameCodeEdit();

    Icd::FrameCodeItemPtr frameCode() const;

signals:

public slots:

private:
};

}

#endif // FRAMECODE_EDIT_H
