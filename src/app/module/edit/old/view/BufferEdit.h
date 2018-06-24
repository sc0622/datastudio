#ifndef BUFFER_EDIT_H
#define BUFFER_EDIT_H

#include "ItemEdit.h"
#include "KernelClass/icdcustomizeddata.h"

class BufferEdit : public ObjectEdit
{
    Q_OBJECT
public:
    explicit BufferEdit(QWidget *parent = nullptr);

signals:

private slots:

    // ObjectEdit interface
protected:
    int windowType() const override;
    bool onEditFinished() override;
    bool onTextChanged(const QString &text) override;
    bool init() override;
    void enableConnect(bool enabled) override;
    ICDCustomizedData *data() override;
    ICDCustomizedData *oldData() override;

private:
    QSpinBox *spinLength_;
    LimitTextEdit *edtDescribe_;
};

#endif // BUFFER_EDIT_H
