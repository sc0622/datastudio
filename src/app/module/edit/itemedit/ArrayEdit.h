#ifndef ARRAY_EDIT_H
#define ARRAY_EDIT_H

#include "ItemEdit.h"

namespace Icd {
class ArrayItem;
typedef std::shared_ptr<ArrayItem> ArrayItemPtr;
}

class QSpinBox;

namespace Edit {

// class ArrayEdit

class ArrayEdit : public ItemEdit
{
    Q_OBJECT
public:
    explicit ArrayEdit(const Icd::ArrayItemPtr &array, QWidget *parent = nullptr);
    ~ArrayEdit();

    Icd::ArrayItemPtr array() const;

    bool init() override;

signals:

public slots:

protected:
    void restoreContent(bool recursive = true) override;
    bool validate() override;
    void saveContent() override;

private:
    QComboBox *comboArrayType_;
    QSpinBox *spinCount_;
};

}

#endif // ARRAY_EDIT_H
