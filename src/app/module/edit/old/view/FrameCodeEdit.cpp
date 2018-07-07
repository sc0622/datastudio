#include "precomp.h"
#include "FrameCodeEdit.h"
#include "LimitLineEdit.h"
#include "LimitTextEdit.h"

FrameCodeEdit::FrameCodeEdit(QWidget *parent)
    : ObjectEdit(parent)
{
    comboLength = new QComboBox(this);
    addFormRow(QStringLiteral("���ȣ�"), comboLength);

    comboData_ = new QComboBox(this);
    addFormRow(QStringLiteral("֡���ݣ�"), comboData_);

    enableConnect(true);
}

int FrameCodeEdit::windowType() const
{
    return wdFrameCode;
}

bool FrameCodeEdit::onEditFinished()
{
    if (ObjectEdit::onEditFinished()) {
        return true;
    }

    return false;
}

bool FrameCodeEdit::onTextChanged(const QString& text)
{
    if (ObjectEdit::onTextChanged(text)) {
        return true;
    }

    bool result = false;
    ICDFrameCodeData *frameCodeData = data();
    const QObject *sender = this->sender();
    if (sender == comboLength) {
        frameCodeData->setFrameType(comboLength->itemData(comboLength->currentIndex()).toInt());
        result = true;
    } else if (sender == comboData_) {
        frameCodeData->setDefaultStr(comboData_->itemData(comboData_->currentIndex()).toString().toStdString());
        result = true;
    }

    if (result) {
        enableCommit(true);
    }

    return result;
}


bool FrameCodeEdit::init()
{
    // length
    comboLength->clear();
    comboLength->addItem(QStringLiteral("1�ֽ�"), GlobalDefine::frame8);
    comboLength->addItem(QStringLiteral("2�ֽ�"), GlobalDefine::frame16);
    comboLength->addItem(QStringLiteral("4�ֽ�"), GlobalDefine::frame32);
    int index = comboLength->findData(data()->frameType());
    if (-1 == index) {
        index = 0;
    }
    comboLength->setCurrentIndex(index);
    // data binding
    comboData_->clear();
    QMap<int, QString> frame;
    QVariantList args;
    args.append(qVariantFromValue((void*)&frame));
    QString command("idle");
    args.append(qVariantFromValue((void*)&command));
    jnotify->send("edit.queryFrameData", args);
    // ���Ѱ����ݼ��뱸ѡ
    ICDComplexData::smtComplex complex = data()->data();
    if (complex) {
        frame[complex->serial()] = QString::fromStdString(complex->name());
    }
    comboData_->addItem(QStringLiteral("����"), -1);
    QMapIterator<int, QString> it = frame;
    while (it.hasNext()) {
        it.next();
        comboData_->addItem(QStringLiteral("��<%1>").arg(it.value()), it.key());
    }
    //
    const QString code = QString::fromStdString(data()->defaultStr());
    if (code.isEmpty()) {
        comboData_->setCurrentIndex(0);
    } else {
        int index = comboData_->findData(code);
        comboData_->setCurrentIndex((index < 0) ? 0 : index);
    }

    return true;
}

void FrameCodeEdit::enableConnect(bool enabled)
{
    ObjectEdit::enableConnect(enabled);

    disconnect(comboLength, SIGNAL(currentTextChanged(const QString &)),
               this, SLOT(onTextChanged(const QString &)));
    disconnect(comboData_, SIGNAL(currentTextChanged(const QString &)),
               this, SLOT(onTextChanged(const QString &)));
    if (enabled) {
        connect(comboLength, SIGNAL(currentTextChanged(const QString &)),
                this, SLOT(onTextChanged(const QString &)));
        connect(comboData_, SIGNAL(currentTextChanged(const QString &)),
                this, SLOT(onTextChanged(const QString &)));
    }
}

bool FrameCodeEdit::confirm()
{
    return true;
}

bool FrameCodeEdit::validate()
{
    return true;
}

ICDFrameCodeData *FrameCodeEdit::data()
{
    return static_cast<ICDFrameCodeData*>(ObjectEdit::data());
}

ICDFrameCodeData *FrameCodeEdit::oldData()
{
    return static_cast<ICDFrameCodeData*>(ObjectEdit::oldData());
}
