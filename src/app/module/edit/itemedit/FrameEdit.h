#ifndef FRAME_EDIT_H
#define FRAME_EDIT_H

#include "ItemEdit.h"

namespace Icd {
class FrameItem;
typedef std::shared_ptr<FrameItem> FrameItemPtr;
}

namespace Edit {

// class FrameEdit

class FrameEdit : public ItemEdit
{
    Q_OBJECT
public:
    explicit FrameEdit(const Icd::FrameItemPtr &frame, QWidget *parent = nullptr);
    ~FrameEdit();

    Icd::FrameItemPtr frame() const;

signals:

public slots:

private:
};

}

#endif // FRAME_EDIT_H
