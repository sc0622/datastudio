#ifndef CHECK_EDIT_H
#define CHECK_EDIT_H

#include "ItemEdit.h"

namespace Icd {
class CheckItem;
typedef std::shared_ptr<CheckItem> CheckItemPtr;
}

class QSpinBox;

namespace Edit {

// class CheckEdit

class CheckEdit : public ItemEdit
{
    Q_OBJECT
public:
    explicit CheckEdit(const Icd::CheckItemPtr &check, QWidget *parent = nullptr);
    ~CheckEdit();

    Icd::CheckItemPtr check() const;

    bool init() override;

signals:

public slots:

private:
    QComboBox *comboCheckType_;
    QSpinBox *spinStartPos_;
    QSpinBox *spinEndPos_;
};

}

#endif // CHECK_EDIT_H
