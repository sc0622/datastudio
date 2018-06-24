#include "precomp.h"
#include "TableEdit.h"
#include "KernelClass/globalstruct.h"
#include "LimitLineEdit.h"
#include "LimitTextEdit.h"

TableEdit::TableEdit(QWidget* parent)
    : ObjectEdit(parent)
{
    spinLength_ = new QSpinBox(this);
    spinLength_->setObjectName("edtLength");
    spinLength_->setRange(0, 1e6);
    spinLength_->setSuffix(QStringLiteral(" 字节"));
    checkLength_ = new QCheckBox(QStringLiteral("长度："), this);
    addFormRow(checkLength_, spinLength_);

    enableConnect(true);
}

void TableEdit::onCheckStateChanged(bool checked)
{
    spinLength_->setEnabled(checked);
    data_.check = checked;
    enableCommit(true);
}

int TableEdit::windowType() const
{
    return wdTable;
}

bool TableEdit::onEditFinished()
{
    if (ObjectEdit::onEditFinished()) {
        return true;
    }

    return false;
}

bool TableEdit::onTextChanged(const QString &text)
{
    if (ObjectEdit::onTextChanged(text)) {
        return true;
    }

    if (sender() == spinLength_) {
        data_.nLength = spinLength_->value();
        enableCommit(true);
        return true;
    }

    return false;
}

bool TableEdit::init()
{
    //
    checkLength_->setChecked(data_.check);
    //
    spinLength_->setValue(data_.nLength);
    spinLength_->setEnabled(data_.check);
    spinLength_->clearFocus();

    return true;
}

void TableEdit::enableConnect(bool enabled)
{
    ObjectEdit::enableConnect(enabled);

    disconnect(spinLength_, SIGNAL(valueChanged(const QString &)),
               this, SLOT(onTextChanged(const QString &)));
    disconnect(checkLength_, SIGNAL(toggled(bool)),
               this, SLOT(onCheckStateChanged(bool)));
    if (enabled) {
        connect(spinLength_, SIGNAL(valueChanged(const QString &)),
                this, SLOT(onTextChanged(const QString &)));
        connect(checkLength_, SIGNAL(toggled(bool)),
                this, SLOT(onCheckStateChanged(bool)));
    }
}

bool TableEdit::validate()
{
    if (data_.check) {
        int total = 0;
        QVariantList args;
        args.append(qVariantFromValue((void*)&total));
        QString command("total");
        args.append(qVariantFromValue((void*)&command));
        jnotify->send("edit.queryTableInformation", args);
        if (total > data_.nLength) {
            setStatus(QStringLiteral("已规划数据超过当前预设长度！"));
            return false;
        }
    }

    return true;
}

void *TableEdit::nonData()
{
    return reinterpret_cast<void*>(&data_);
}

void *TableEdit::nonOldData()
{
    return reinterpret_cast<void*>(&oldData_);
}

bool TableEdit::setData(const _UIData &data)
{
    if (!data.data) {
        return false;
    }

    data_ = *reinterpret_cast<stICDBase*>(data.data);
    oldData_ = data_;

    return setExtData(data, true);
}

QString TableEdit::name() const
{
    return QString::fromStdString(data_.sName);
}

void TableEdit::setName(const QString &text)
{
    data_.sName = text.toStdString();
}

QString TableEdit::mark() const
{
    return QString::fromStdString(data_.sCode);
}

void TableEdit::setMark(const QString &text)
{
    data_.sCode = text.toStdString();
}

QString TableEdit::desc() const
{
    return QString::fromStdString(data_.sDescribe);
}

void TableEdit::setDesc(const QString &text)
{
    data_.sDescribe = text.toStdString();
}
