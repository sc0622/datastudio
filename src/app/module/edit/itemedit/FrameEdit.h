#ifndef FRAME_EDIT_H
#define FRAME_EDIT_H

#include "ItemEdit.h"

namespace Icd {
class FrameItem;
typedef std::shared_ptr<FrameItem> FrameItemPtr;
}

class QSpinBox;

namespace Edit {

// class FrameEdit

class FrameEdit : public ItemEdit
{
    Q_OBJECT
public:
    explicit FrameEdit(const Icd::FrameItemPtr &frame, QWidget *parent = nullptr);
    ~FrameEdit();

    Icd::FrameItemPtr frame() const;

    bool init() override;

signals:

public slots:

protected:
    void restoreContent(bool recursive = true) override;
    bool validate() override;
    void saveContent() override;

private:
    QSpinBox *spinBoxSequence_;
};

}

#endif // FRAME_EDIT_H
