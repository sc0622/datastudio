#include "precomp.h"
#include "SystemEdit.h"
#include "LimitLineEdit.h"
#include "LimitTextEdit.h"

SystemEdit::SystemEdit(QWidget *parent)
    : ObjectEdit(parent)
{

}

int SystemEdit::windowType() const
{
    return wdSystem;
}

void *SystemEdit::nonData()
{
    return &data_;
}

void *SystemEdit::nonOldData()
{
    return &oldData_;
}

bool SystemEdit::setData(const _UIData &data)
{
    if (!data.data) {
        return false;
    }

    data_ = *reinterpret_cast<stSystem*>(data.data);
    oldData_ = data_;

    return setExtData(data, true);
}

QString SystemEdit::name() const
{
    return QString::fromStdString(data_.sName);
}

void SystemEdit::setName(const QString &text)
{
    data_.sName = text.toStdString();
}

QString SystemEdit::mark() const
{
    return QString::fromStdString(data_.sSign);
}

void SystemEdit::setMark(const QString &text)
{
    data_.sSign = text.toStdString();
}

QString SystemEdit::desc() const
{
    return QString::fromStdString(data_.sDescribe);
}

void SystemEdit::setDesc(const QString &text)
{
    data_.sDescribe = text.toStdString();
}
