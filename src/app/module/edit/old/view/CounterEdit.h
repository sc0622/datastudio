#ifndef COUNTER_EDIT_H
#define COUNTER_EDIT_H

#include "ItemEdit.h"
#include "KernelClass/icdcounterdata.h"

class CounterEdit : public ObjectEdit
{
    Q_OBJECT
public:
    explicit CounterEdit(QWidget *parent = nullptr);

signals:

public slots:

    // ObjectEdit interface
protected:
    int windowType() const override;
    bool onEditFinished() override;
    bool onTextChanged(const QString &text) override;
    bool init() override;
    void enableConnect(bool enabled) override;
    bool confirm() override;
    bool validate();
    ICDCounterData *data() override;
    ICDCounterData *oldData() override;

private:
    QComboBox *comboCounterType_;
};

#endif // CounterEdit_H
