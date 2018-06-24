#ifndef FRAMECODE_EDIT_H
#define FRAMECODE_EDIT_H

#include "ItemEdit.h"
#include "KernelClass/icdframecodedata.h"

class FrameCodeEdit : public ObjectEdit
{
    Q_OBJECT
public:
    explicit FrameCodeEdit(QWidget *parent = nullptr);

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
    ICDFrameCodeData *data() override;
    ICDFrameCodeData *oldData() override;

private:
    QComboBox *comboLength;
    QComboBox *comboData_;
};

#endif // FRAMECODE_EDIT_H
