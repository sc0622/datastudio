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
    ~CounterEdit() override;

    Icd::CounterItemPtr counter() const;

    bool init() override;

signals:

public slots:

protected:
    void restoreContent(bool recursive = true) override;
    bool validate() const override;
    void saveContent() override;

private:
    QComboBox *comboCounterType_;
};

}

#endif // COUNTER_EDIT_H
