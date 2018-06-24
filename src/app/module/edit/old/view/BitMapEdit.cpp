#include "precomp.h"
#include "BitMapEdit.h"
#include "jwt/jxmltable.h"
#include "jwt/jsplitter.h"
#include "LimitLineEdit.h"
#include "LimitTextEdit.h"

BitMapEdit::BitMapEdit(QWidget *parent)
    : ObjectEdit(parent)
{
    spinBitStart_ = new QSpinBox(this);
    spinBitStart_->setRange(0, MAX_BIT_COUNT - 1);
    addFormRow(QStringLiteral("<font color=red>*</font>��ʼλ��"), spinBitStart_);

    spinBitCount_ = new QSpinBox(this);
    spinBitCount_->setRange(1, MAX_BIT_COUNT);
    addFormRow(QStringLiteral("<font color=red>*</font>���ȣ�"), spinBitCount_);

    editDefault_ = new QLineEdit(this);
    editDefault_->setValidator(new QRegExpValidator(QRegExp("[0-1]{,64}")));
    editDefault_->setMaxLength(MAX_BIT_COUNT);
    editDefault_->setPlaceholderText(QStringLiteral("������"));
    editDefault_->setToolTip(QStringLiteral("�����ƣ�"));
    addFormRow(QStringLiteral("Ĭ��ֵ��"), editDefault_);

    QGroupBox* groupSpecs = new QGroupBox(QStringLiteral("��������Ϣ"), this);
    splitterBase()->addWidget(groupSpecs);

    QVBoxLayout* layoutSpecs = new QVBoxLayout(groupSpecs);
    layoutSpecs->setContentsMargins(0, 0, 0, 0);
    layoutSpecs->setSpacing(1);

    tableView_ = new JXmlTable(this);
    tableView_->verticalHeader()->setFixedWidth(40);
    tableView_->verticalHeader()->setDefaultAlignment(Qt::AlignCenter);
    tableView_->horizontalHeader()->setSectionsMovable(false);
    layoutSpecs->addWidget(tableView_);

    tableView_->loadConfig(JMain::instance()->configDir().append("/old/xmltable.xml"),
                           "EigenvalueMapTable");
    enableConnect(true);
}

int BitMapEdit::windowType() const
{
    return wdBitMap;
}

bool BitMapEdit::onEditFinished()
{
    if (ObjectEdit::onEditFinished()) {
        return true;
    }

    bool result = false;
    const QObject *sender = this->sender();
    if (sender == spinBitStart_) {
        spinBitCount_->setMaximum(MAX_BIT_COUNT - spinBitStart_->value());
        result = true;
    } else if (sender == spinBitCount_) {
        editDefault_->setMaxLength(spinBitCount_->value());
        result = true;
    } else if (sender == tableView_) {
        enableCommit(true);
        return true;
    }

    if (result) {
        const int count = spinBitCount_->value();
        if (count > 0) {
            editDefault_->setMaxLength(count);
            updateTable(count, (sender == spinBitCount_));
        }
        enableCommit(true);
    }

    return result;
}

bool BitMapEdit::onTextChanged(const QString &text)
{
    if (ObjectEdit::onTextChanged(text)) {
        return true;
    }

    bool result = false;
    ICDBitData *bitData = data();
    const QObject *sender = this->sender();
    if (sender == editDefault_) {
        bitData->setDefaultStr(QString::number(text.toLongLong(0, 2)).toStdString());
        result = true;
    } else if (sender == spinBitStart_) {
        bitData->setStart(text.toInt());
        bitData->setBitLength(spinBitCount_->value());
        result = true;
    } else if (sender == spinBitCount_) {
        bitData->setBitLength(spinBitCount_->value());
        result = true;
    }

    if (result) {
        enableCommit(true);
    }

    return result;
}

bool BitMapEdit::init()
{
    spinBitStart_->setProperty("highlight", false);
    spinBitStart_->setValue(data()->start());

    spinBitCount_->setProperty("highlight", false);
    spinBitCount_->setValue(data()->length());

    editDefault_->setProperty("highlight", false);
    editDefault_->setMaxLength(data()->length());
    editDefault_->setText(QString::number(QString::fromStdString
                                          (data()->defaultStr()).toULongLong(), 2));

    std::map<double, std::string> items = data()->values();
    std::map<double, std::string>::iterator iter = items.begin();
    tableView_->clearContents();
    _Eigenvalue _data;
    const int bound = data()->length();
    for (int i = 0; i < bound; ++i) {
        _data.value = i;
        updateMapOne(i, _data, GlobalDefine::optNew);
    }
    for (int start = data()->start(); iter != items.end(); ++iter) {
        _data.value = iter->first;
        _data.describe = iter->second.c_str();
        updateMapOne(_data.value - start, _data,  GlobalDefine::optEdit);
    }

    return true;
}

void BitMapEdit::enableConnect(bool enabled)
{
    ObjectEdit::enableConnect(enabled);

    disconnect(spinBitStart_, SIGNAL(valueChanged(const QString &)),
               this, SLOT(onTextChanged(const QString &)));
    disconnect(spinBitStart_, SIGNAL(editingFinished()),
               this, SLOT(onEditFinished()));
    disconnect(spinBitCount_, SIGNAL(valueChanged(const QString &)),
               this, SLOT(onTextChanged(const QString &)));
    disconnect(spinBitCount_, SIGNAL(editingFinished()),
               this, SLOT(onEditFinished()));
    disconnect(editDefault_, SIGNAL(textChanged(const QString &)),
               this, SLOT(onTextChanged(const QString &)));
    disconnect(tableView_, SIGNAL(contentChanged()),
               this, SLOT(onEditFinished()));
    if (enabled) {
        connect(spinBitStart_, SIGNAL(valueChanged(const QString &)),
                this, SLOT(onTextChanged(const QString &)));
        connect(spinBitStart_, SIGNAL(editingFinished()),
                this, SLOT(onEditFinished()));
        connect(spinBitCount_, SIGNAL(valueChanged(const QString &)),
                this, SLOT(onTextChanged(const QString &)));
        connect(spinBitCount_, SIGNAL(editingFinished()),
                this, SLOT(onEditFinished()));
        connect(editDefault_, SIGNAL(textChanged(const QString &)),
                this, SLOT(onTextChanged(const QString &)));
        connect(tableView_, SIGNAL(contentChanged()),
                this, SLOT(onEditFinished()));
    }
}

bool BitMapEdit::confirm()
{
    data()->setValues(tableValues());
    return true;
}

bool BitMapEdit::validate()
{
    // ���Ĭ��ֵ
    if (editDefault_->text().length() > spinBitCount_->value()) {
        editDefault_->setFocus();
        editDefault_->setProperty("highlight", true);
        setStatus(QStringLiteral("Ĭ��ֵ������ֹ��Χ��"));
        return false;
    } else {
        editDefault_->setProperty("highlight", false);
    }
    // ��������
    bool error = false;
    const int count = tableView_->rowCount();
    for (int i = 0; i < count; ++i) {
        if (tableView_->itemValue(i, 2).toString().contains(",")
                || tableView_->itemValue(i, 3).toString().contains(",")) {
            error = true;
            break;
        }
    }
    if (error) {
        tableView_->setFocus();
        tableView_->setProperty("highlight", true);
        setStatus(QStringLiteral("ֵ�����в����д˷��š�,��"));
        return false;
    } else {
        tableView_->setProperty("highlight", false);
    }

    return true;
}

ICDBitData *BitMapEdit::data()
{
    return static_cast<ICDBitData*>(ObjectEdit::data());
}

ICDBitData *BitMapEdit::oldData()
{
    return static_cast<ICDBitData*>(ObjectEdit::oldData());
}

void BitMapEdit::updateTable(int newCount, bool behind)
{
    int count = tableView_->rowCount();
    // ����ж����ɾ��
    if (newCount < count) {    // ɾ������
        if (behind) {   // ��ֹλ�����ɾ����󲿷�����
            tableView_->removeRow(newCount, count - newCount);
        } else {    // ��ʼλ�����ɾ����ǰ��������
            tableView_->removeRow(0, count - newCount);
        }
    } else {    // ��ȫ����
        int offset = 0; // ƫ����
        int insertCount = newCount - count; // ����������
        if (behind) {
            offset = count; // ��ֹλ�����ƫ����count
        }
        _Eigenvalue _data;
        /*data.describe = QStringLiteral("����")*/;
        for (int i = 0; i < insertCount; ++i) {
            updateMapOne(i + offset, _data);
        }
        // ���µ�һ������
        tableView_->setItemValue(0, 0, data()->start());
    }
}

// ��bitMap�����һ������
void BitMapEdit::updateMapOne(int index, const _Eigenvalue &data, int option)
{
    if (GlobalDefine::optNew == option) {
        tableView_->insertRow(index);
    } else if (GlobalDefine::optEdit == option) {
        if (index > tableView_->rowCount() - 1) {
            return;
        }
    }

    QStringList lst = data.describe.split(":");
    QStringList desc = lst.size() > 1 ? lst.last().split(";") : QStringList();
    tableView_->setItemValue(index, 0, spinBitStart_->value() + index);
    tableView_->setItemValue(index, 1, lst.first());
    if (!desc.empty()) {
        tableView_->setItemValue(index, 2, desc.first());
        tableView_->setItemValue(index, 3, desc.last());
    }
}

std::map<double, std::string> BitMapEdit::tableValues() const
{
    std::map<double, std::string> result;
    if (!tableView_) {
        return result;
    }

    QString desc;
    const int count = tableView_->rowCount();
    for (int i = 0; i < count; ++i) {
        desc = tableView_->itemValue(i, 1).toString().trimmed();
        desc.append(":").append(tableView_->itemValue(i, 2).toString().trimmed());
        desc.append(";").append(tableView_->itemValue(i, 3).toString().trimmed());
        if (desc.endsWith(":;")) {
            desc = desc.left(desc.length() - 2);
        }
        if (!desc.isEmpty()) {
            result[tableView_->itemValue(i, 0).toInt()] = desc.toStdString();
        }
    }

    return result;
}
