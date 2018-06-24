#ifndef FRAME_EDIT_H
#define FRAME_EDIT_H

#include "ItemEdit.h"
#include "KernelClass/icdcomplexdata.h"

class FrameEdit : public ObjectEdit
{
    Q_OBJECT
public:
    explicit FrameEdit(QWidget *parent = nullptr);

signals:

public slots:

    // ObjectEdit interface
protected:
    int windowType() const override;
    bool onEditFinished() override;
    bool onTextChanged(const QString &text) override;
    bool init() override;
    void enableConnect(bool enabled) override;
    ICDComplexData *data() override;
    ICDComplexData *oldData() override;

private:
    QSpinBox *spinSequence_;
};

#endif // FRAME_EDIT_H
