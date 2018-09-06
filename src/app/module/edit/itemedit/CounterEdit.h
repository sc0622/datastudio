#ifndef COUNTER_EDIT_H
#define COUNTER_EDIT_H

#include "ItemEdit.h"

namespace Icd {
class CounterItem;
typedef std::shared_ptr<CounterItem> CounterItemPtr;
}

namespace Edit {

// class CounterEdit

class CounterEdit : public ItemEdit
{
    Q_OBJECT
public:
    explicit CounterEdit(const Icd::CounterItemPtr &counter, QWidget *parent = nullptr);
    ~CounterEdit();

    Icd::CounterItemPtr counter() const;

    bool init() override;

signals:

public slots:

private:
    QComboBox *comboCounterType_;
};

}

#endif // COUNTER_EDIT_H
