#ifndef SYSTEM_EDIT_H
#define SYSTEM_EDIT_H

#include "ItemEdit.h"
#include "KernelClass/globalstruct.h"

class QGridLayout;

class SystemEdit : public ObjectEdit
{
    Q_OBJECT
public:
    explicit SystemEdit(QWidget *parent = nullptr);

signals:

public slots:

    // ObjectEdit interface
protected:
    int windowType() const override;
    void *nonData() override;
    void *nonOldData() override;
    bool setData(const _UIData &data) override;

    QString name() const override;
    void setName(const QString &text) override;

    QString mark() const override;
    void setMark(const QString &text) override;

    QString desc() const override;
    void setDesc(const QString &text) override;

private:
    stSystem data_;
    stSystem oldData_;
};

#endif // SYSTEM_EDIT_H
