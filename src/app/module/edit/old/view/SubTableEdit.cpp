#include "precomp.h"
#include "SubTableEdit.h"
#include "KernelClass/globalstruct.h"
#include "LimitLineEdit.h"
#include "LimitTextEdit.h"
#include "HexSpinBox.h"
#include "jwt/jspinbox.h"

SubTableEdit::SubTableEdit(QWidget* parent)
    : ObjectEdit(parent)
{
    comboLength_ = new QComboBox(this);
    addFormRow(QStringLiteral("帧码长度："), comboLength_);

    spinCode_ = new JLargeSpinBox(this);
    spinCode_->setObjectName("edtCode");
    spinCode_->setRadix(16);
    spinCode_->setRange(0, 0xff);
    spinCode_->setPrefix("0x");
    spinCode_->setFillChar(QChar('0'));
    addFormRow(QStringLiteral("帧码："), spinCode_);

    spinSequence_ = new QSpinBox(this);
    spinSequence_->setRange(1, 1e9);
    addFormRow(QStringLiteral("时序："), spinSequence_);

    enableConnect(true);
}

void SubTableEdit::onCodeChanged(const qulonglong &value)
{
    Q_UNUSED(value);
    data_.sCode = spinCode_->text().remove(spinCode_->prefix()).toStdString();
    enableCommit(true);
}

int SubTableEdit::windowType() const
{
    return wdSubTable;
}

bool SubTableEdit::onEditFinished()
{
    if (ObjectEdit::onEditFinished()) {
        return true;
    }

    return false;
}

bool SubTableEdit::onTextChanged(const QString &text)
{
    if (ObjectEdit::onTextChanged(text)) {
        return true;
    }

    bool result = false;
    const QObject *sender = this->sender();
    if (sender == comboLength_) {
        int length = 2 * comboLength_->currentData().toInt();
        QString strValue = QString().fill('f', length);
        spinCode_->setRange(0, strValue.prepend("0x").toUInt(0, 16));
        length -= data_.sCode.length();
        if (length > 0) {
            data_.sCode.insert(0, length, '0');
        } else if (length < 0) {
            data_.sCode = data_.sCode.substr(abs(length));
        }
        spinCode_->setValue(QString(data_.sCode.c_str()).toInt(0, 16));
        result = true;
    } else if (sender == spinSequence_) {
        data_.sRemark = QString::number(spinSequence_->value()).toStdString();
        result = true;
    }

    if (result) {
        enableCommit(true);
    }

    return result;
}

bool SubTableEdit::init()
{
    // length
    comboLength_->clear();
    comboLength_->addItem(QStringLiteral("1字节"), 1);
    comboLength_->addItem(QStringLiteral("2字节"), 2);
    comboLength_->addItem(QStringLiteral("4字节"), 4);
    if (comboLength_) {
        comboLength_->setCurrentIndex(comboLength_->findData(data_.sCode.length() / 2));
    }
    // code
    QString strValue = QString().fill('f', data_.sCode.length());
    spinCode_->setRange(0, strValue.prepend("0x").toUInt(0, 16));
    spinCode_->setValue(QString(data_.sCode.c_str()).toInt(0, 16));
    spinCode_->clearFocus();
    data_.sCode = spinCode_->text().remove(spinCode_->prefix()).toStdString();
    // sequence;
    spinSequence_->setValue(QString::fromStdString(data_.sRemark).toInt());

    return true;
}

void SubTableEdit::enableConnect(bool enabled)
{
    ObjectEdit::enableConnect(enabled);

    disconnect(comboLength_, SIGNAL(currentTextChanged(const QString &)),
               this, SLOT(onTextChanged(const QString &)));
    disconnect(spinCode_, SIGNAL(valueChanged(const qulonglong &)),
               this, SLOT(onCodeChanged(const qulonglong &)));
    disconnect(spinSequence_, SIGNAL(valueChanged(const QString &)),
               this, SLOT(onTextChanged(const QString &)));
    if (enabled) {
        connect(comboLength_, SIGNAL(currentTextChanged(const QString &)),
                this, SLOT(onTextChanged(const QString &)));
        connect(spinCode_, SIGNAL(valueChanged(const qulonglong &)),
                this, SLOT(onCodeChanged(const qulonglong &)));
        connect(spinSequence_, SIGNAL(valueChanged(const QString &)),
                this, SLOT(onTextChanged(const QString &)));
    }
}

bool SubTableEdit::confirm()
{
    return true;
}

bool SubTableEdit::validate()
{
    if (data_.sCode.empty()) {
        spinCode_->setFocus();
        spinCode_->setProperty("highlight", true);
        setStatus(QStringLiteral("帧码不能为空！"));
        return false;
    } else {

        QString section = "code";
        QMap<QString, QString> existed;
        QVariantList args;
        args.append(qVariantFromValue((void*)&existed));
        args.append(qVariantFromValue((void*)&section));
        jnotify->send("edit.queryExistedData", args);
        if (existed.contains(data_.sCode.c_str())) {
            spinCode_->setFocus();
            spinCode_->setProperty("highlight", true);
            setStatus(QStringLiteral("已存在同名标识！"));
            return false;
        } else {
            if (data_.sCode.size() < 2) {
                data_.sCode.insert(0, "0");
                spinCode_->setValue(QString(data_.sCode.c_str()).toInt(0, 16));
            }
            spinCode_->clearFocus();
            spinCode_->setProperty("highlight", false);
        }
    }

    return true;
}

void *SubTableEdit::nonData()
{
    return &data_;
}

void *SubTableEdit::nonOldData()
{
    return &oldData_;
}

bool SubTableEdit::setData(const _UIData &data)
{
    if (!data.data) {
        return false;
    }

    data_ = *reinterpret_cast<stICDBase*>(data.data);
    oldData_ = data_;

    return setExtData(data, true);
}

QString SubTableEdit::name() const
{
    return QString::fromStdString(data_.sName);
}

void SubTableEdit::setName(const QString &text)
{
    data_.sName = text.toStdString();
}

QString SubTableEdit::mark() const
{
    return QString::fromStdString(data_.sCode);
}

void SubTableEdit::setMark(const QString &text)
{
    data_.sCode = text.toStdString();
}

QString SubTableEdit::desc() const
{
    return QString::fromStdString(data_.sDescribe);
}

void SubTableEdit::setDesc(const QString &text)
{
    data_.sDescribe = text.toStdString();
}
