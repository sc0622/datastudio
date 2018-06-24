#ifndef TABLE_EDIT_H
#define TABLE_EDIT_H

#include "ItemEdit.h"

class TableEdit : public ObjectEdit
{
    Q_OBJECT
public:
    explicit TableEdit(QWidget* parent = nullptr);

signals:

public slots:
    void onCheckStateChanged(bool checked);

    // ObjectEdit interface
protected:
    int windowType() const override;
    bool onEditFinished() override;
    bool onTextChanged(const QString &text) override;
    bool init() override;
    void enableConnect(bool enabled) override;
    bool validate();
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
    stICDBase data_;
    stICDBase oldData_;
    QSpinBox *spinLength_;
    QCheckBox *checkLength_;
};

#endif // TABLE_EDIT_H
