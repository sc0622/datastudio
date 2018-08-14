#ifndef SUBTABLE_EDIT_H
#define SUBTABLE_EDIT_H

#include "ItemEdit.h"

class JLargeSpinBox;

class SubTableEdit : public ObjectEdit
{
    Q_OBJECT
public:
    explicit SubTableEdit(QWidget* parent = nullptr);

signals:

public slots:
    void onCodeChanged(const qulonglong &value);

    // ObjectEdit interface
protected:
    int windowType() const override;
    bool onEditFinished() override;
    bool onTextChanged(const QString &text) override;
    bool init() override;
    void enableConnect(bool enabled) override;
    bool confirm() override;
    bool validate() override;
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
    QComboBox *comboLength_;
    JLargeSpinBox *spinCode_;
    QSpinBox *spinSequence_;
};

#endif // SUBTABLE_EDIT_H
