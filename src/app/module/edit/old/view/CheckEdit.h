#ifndef CHECK_EDIT_H
#define CHECK_EDIT_H

#include "ItemEdit.h"
#include "KernelClass/icdcheckdata.h"

class CheckEdit : public ObjectEdit
{
    Q_OBJECT
public:
    explicit CheckEdit(QWidget *parent = nullptr);

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
    ICDCheckData *data() override;
    ICDCheckData *oldData() override;

private:
    QComboBox *comboCheckType_;
    QSpinBox *spinStart_;
    QSpinBox *spinEnd_;
};

#endif // CHECK_EDIT_H
