#ifndef HEADER_EDIT_H
#define HEADER_EDIT_H

#include "ItemEdit.h"
#include "KernelClass/icdcommondata.h"

class HexSpinBox;

class HeaderEdit : public ObjectEdit
{
    Q_OBJECT
public:
    explicit HeaderEdit(QWidget *parent = nullptr);

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
    ICDCommonData *data() override;
    ICDCommonData *oldData() override;

private:
    HexSpinBox *spinDefault_;
};

#endif // HEADER_EDIT_H
