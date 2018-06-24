#ifndef COMPLEX_EDIT_H
#define COMPLEX_EDIT_H

#include "ItemEdit.h"
#include "KernelClass/icdcomplexdata.h"

class ComplexEdit : public ObjectEdit
{
    Q_OBJECT
public:
    explicit ComplexEdit(QWidget *parent = nullptr);

signals:

private slots :

    // ObjectEdit interface
protected:
    int windowType() const override;
    bool onEditFinished() override;
    bool onTextChanged(const QString &text) override;
    bool init() override;
    void enableConnect(bool enabled) override;
    bool confirm() override;
    bool validate();
    ICDComplexData *data() override;
    ICDComplexData *oldData() override;

private:
    QSpinBox *spinLength_;
};

#endif // COMPLEX_EDIT_H
