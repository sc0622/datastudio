#ifndef ARRAY_EDIT_H
#define ARRAY_EDIT_H

#include "ItemEdit.h"
#include "KernelClass/icdarraydata.h"

class QSpinBox;

class ArrayEdit : public ObjectEdit
{
    Q_OBJECT
public:
    explicit ArrayEdit(QWidget *parent = nullptr);

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
    ICDArrayData *data() override;
    ICDArrayData *oldData() override;

private:
    QComboBox *comboArrayType_;
    QSpinBox *spinCount_;
    LimitTextEdit *editRemak;
};

#endif // ARRAY_EDIT_H
