#ifndef HEADER_EDIT_H
#define HEADER_EDIT_H

#include "ItemEdit.h"

namespace Icd {
class HeaderItem;
typedef std::shared_ptr<HeaderItem> HeaderItemPtr;
}

class JSpinBox;

namespace Edit {

// class HeaderEdit

class HeaderEdit : public ItemEdit
{
    Q_OBJECT
public:
    explicit HeaderEdit(const Icd::HeaderItemPtr &header, QWidget *parent = nullptr);
    ~HeaderEdit();

    Icd::HeaderItemPtr header() const;

    bool init() override;

signals:

public slots:

private:
    JSpinBox *spinDefault_;
};

}

#endif // HEADER_EDIT_H
