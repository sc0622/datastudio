#include "precomp.h"
#include "VehicleEdit.h"
#include "LimitLineEdit.h"
#include "LimitTextEdit.h"

VehicleEdit::VehicleEdit(QWidget *parent)
    : ObjectEdit(parent)
{

}

int VehicleEdit::windowType() const
{
    return wdVehicle;
}

void *VehicleEdit::nonData()
{
    return &data_;
}

void *VehicleEdit::nonOldData()
{
    return &oldData_;
}

bool VehicleEdit::setData(const _UIData &data)
{
    if (!data.data) {
        return false;
    }

    data_ = *reinterpret_cast<stPlane*>(data.data);
    oldData_ = data_;

    return setExtData(data, true);
}

QString VehicleEdit::name() const
{
    return QString::fromStdString(data_.sName);
}

void VehicleEdit::setName(const QString &text)
{
    data_.sName = text.toStdString();
}

QString VehicleEdit::mark() const
{
    return QString::fromStdString(data_.sSign);
}

void VehicleEdit::setMark(const QString &text)
{
    data_.sSign = text.toStdString();
}

QString VehicleEdit::desc() const
{
    return QString::fromStdString(data_.sDescribe);
}

void VehicleEdit::setDesc(const QString &text)
{
    data_.sDescribe = text.toStdString();
}
