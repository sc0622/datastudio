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

    bool init() override;

signals:

public slots:

protected:
    void restoreContent(bool recursive = true) override;
    bool validate() override;
    void saveContent() override;

private:
    bool initBinding();
    bool restoreBinding();

private:
    QComboBox *comboFrameCodeType_;
    QComboBox *comboBinding_;
};

}

#endif // FRAMECODE_EDIT_H
