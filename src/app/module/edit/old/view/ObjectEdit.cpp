#include "precomp.h"
#include "ObjectEdit.h"
#include "HeaderEdit.h"
#include "CounterEdit.h"
#include "CheckEdit.h"
#include "FrameCodeEdit.h"
#include "ItemEdit.h"
#include "BitMapEdit.h"
#include "BitValueEdit.h"
#include "FrameEdit.h"
#include "ComplexEdit.h"
#include "BufferEdit.h"
#include "ArrayEdit.h"
#include "NumericEdit.h"
#include "SubTableEdit.h"
#include "TableEdit.h"
#include "SystemEdit.h"
#include "VehicleEdit.h"
#include "limittextedit.h"

ObjectEdit::ObjectEdit(QWidget *parent)
    : QWidget(parent)
    , isBaseData_(false)
{
    layoutMain_ = new QVBoxLayout(this);
    layoutMain_->setContentsMargins(0, 0, 0, 0);
    layoutMain_->setSpacing(1);

    splitterBase_ = new JSplitter(Qt::Vertical, this);
    splitterBase_->setScales(QList<double>() << 1 << 2);
    splitterBase_->setHandleWidth(3);
    layoutMain_->addWidget(splitterBase_);

    groupBoxBase_ = new QGroupBox(tr("Base information"), this);
    splitterBase_->addWidget(groupBoxBase_);

    layoutBase_ = new QFormLayout(groupBoxBase_);
    layoutBase_->setLabelAlignment(Qt::AlignRight);
    layoutBase_->setContentsMargins(3, 3, 3, 3);

    editName_ = new QLineEdit(this);
    editName_->setMaxLength(255);
    editName_->setToolTip(tr("At most 255 bytes"));
    layoutBase_->addRow(tr("<font color=red>*</font>Name:"), editName_);

    editMark_ = new QLineEdit(this);
    editMark_->setMaxLength(255);
    editMark_->setToolTip(QStringLiteral("At most 255 bytes"));
    editMark_->setValidator(new QRegExpValidator(QRegExp("([a-zA-Z_]){1}([a-zA-Z0-9_]){,254}")));
    layoutBase_->addRow(tr("<font color=red>*</font>Identity:"), editMark_);

    editDesc_ = new LimitTextEdit(this);
    editDesc_->setMaxLength(255);
    editDesc_->setToolTip(tr("At most 255 bytes"));
    layoutBase_->addRow(tr("Describe:"), editDesc_);

    QHBoxLayout *layoutButton = new QHBoxLayout();
    layoutMain_->addLayout(layoutButton);

    layoutButton->addStretch();
    // apply
    buttonConfirm_ = new QPushButton(QIcon(":/datastudio/image/global/apply.png"),
                                     tr("Commit"), this);
    buttonConfirm_->setFixedWidth(100);
    layoutButton->addWidget(buttonConfirm_);
    // cancel
    buttonCancel_ = new QPushButton(QIcon(":/datastudio/image/global/cancel.png"),
                                    tr("Cancel"), this);
    buttonCancel_->setFixedWidth(100);
    layoutButton->addSpacing(30);
    layoutButton->addWidget(buttonCancel_);
    layoutButton->addStretch();

    editStatus_ = new QLineEdit(this);
    editStatus_->setObjectName("__status__");
    editStatus_->setFixedHeight(22);
    editStatus_->setEnabled(false);
    layoutMain_->addWidget(editStatus_);

    connect(buttonConfirm_, &QPushButton::clicked, this, [=](){
        if (data_) {
            QVariantList args;
            args.append(qVariantFromValue(static_cast<void*>(&data_)));
            jnotify->send("edit.fillBitSerial", args);
        }

        if (!_validate()) {
            return;
        }

        if (!confirm()) {
            return;
        }

        bool result = false;
        emit confirmed(result);
        if (result) {
            enableCommit(false);
        } else {
            setStatus(tr("Save data failure!"));
        }
    });
    connect(buttonCancel_, &QPushButton::clicked, this, [=](){
        _UIData data;
        data.data = nonOldData();
        setExtData(data, isBaseData_);
        cancel();
        enableCommit(false);
    });

    enableConnect(true);
}

ObjectEdit::~ObjectEdit()
{

}

int ObjectEdit::windowType() const
{
    return wdUnknown;
}

void ObjectEdit::changeType(int type)
{
    if (data_) {
        data_->setType(type);
        enableCommit(true);
    }
}

int ObjectEdit::originalType() const
{
    if (oldData_) {
        return oldData_->type();
    } else {
        return GlobalDefine::dtInvalid;
    }
}

void *ObjectEdit::nonData()
{
    if (!data_) {
        return nullptr;
    }

    return reinterpret_cast<void*>(&data_);
}

void *ObjectEdit::nonOldData()
{
    if (!oldData_) {
        return nullptr;
    }

    return reinterpret_cast<void*>(&oldData_);
}

bool ObjectEdit::setData(const _UIData &data)
{
    return setExtData(data, false);
}

void ObjectEdit::enableCommit(bool enabled)
{
    buttonConfirm_->setEnabled(enabled);
    buttonCancel_->setEnabled(enabled);
}

ObjectEdit *ObjectEdit::create(int windowType)
{
    switch (windowType) {
    case wdHeader: return new HeaderEdit();
    case wdCounter: return new CounterEdit();
    case wdCheck: return new CheckEdit();
    case wdFrameCode: return new FrameCodeEdit();
    case wdItem: return new ItemEdit();
    case wdArray: return new ArrayEdit();
    case wdNumeric: return new NumericEdit();
    case wdBitMap: return new BitMapEdit();
    case wdBitValue: return new BitValueEdit();
    case wdFrame: return new FrameEdit();
    case wdComplex: return new ComplexEdit();
    case wdBuffer: return new BufferEdit();
    case wdSubTable: return new SubTableEdit();
    case wdTable: return new TableEdit();
    case wdSystem: return new SystemEdit();
    case wdVehicle: return new VehicleEdit();
    default: return new ObjectEdit();
    }
}


bool ObjectEdit::onEditFinished()
{
    bool result = false;
    const QObject *sender = this->sender();
    if (sender == editDesc_) {
        setDesc(editDesc_->toPlainText().trimmed());
        result = true;
    }

    if (result) {
        enableCommit(true);
    }

    return result;
}

bool ObjectEdit::onTextChanged(const QString &text)
{
    bool result = false;
    const QObject *sender = this->sender();
    if (sender == editName_) {
        setName(text.trimmed());
        result = true;
    } else if (sender == editMark_) {
        setMark(text.trimmed());
       result = true;
    } else if (sender == editDesc_) {
        setDesc(text.trimmed());
        result = true;
    } if (sender == editStatus_) {
        enableConfirm(text.trimmed().isEmpty());
    }

    if (result) {
        enableCommit(true);
    }

    return result;
}

QVBoxLayout *ObjectEdit::layoutMain() const
{
    return layoutMain_;
}

JSplitter *ObjectEdit::splitterBase() const
{
    return splitterBase_;
}

QFormLayout *ObjectEdit::layoutBase() const
{
    return layoutBase_;
}

void ObjectEdit::addFormRow(QWidget *label, QWidget *field) const
{
    layoutBase_->insertRow(layoutBase_->rowCount() - 1, label, field);
    layoutBase_->setAlignment(label, Qt::AlignVCenter);
}

void ObjectEdit::addFormRow(QWidget *label, QLayout *field) const
{
    layoutBase_->insertRow(layoutBase_->rowCount() - 1, label, field);
    layoutBase_->setAlignment(label, Qt::AlignVCenter);
}

void ObjectEdit::addFormRow(const QString &labelText, QWidget *field) const
{
    layoutBase_->insertRow(layoutBase_->rowCount() - 1, labelText, field);
}

void ObjectEdit::addFormRow(const QString &labelText, QLayout *field) const
{
    layoutBase_->insertRow(layoutBase_->rowCount() - 1, labelText, field);
}

bool ObjectEdit::init()
{
    return true;
}

void ObjectEdit::enableConnect(bool enabled)
{
    disconnect(editName_, &QLineEdit::textChanged, this, &ObjectEdit::onTextChanged);
    disconnect(editMark_, &QLineEdit::textChanged, this, &ObjectEdit::onTextChanged);
    disconnect(editDesc_, &QTextEdit::textChanged, this, &ObjectEdit::onEditFinished);
    disconnect(editStatus_, &QLineEdit::textChanged, this, &ObjectEdit::onTextChanged);
    if (enabled) {
        connect(editName_, &QLineEdit::textChanged, this, &ObjectEdit::onTextChanged);
        connect(editMark_, &QLineEdit::textChanged, this, &ObjectEdit::onTextChanged);
        connect(editDesc_, &QTextEdit::textChanged, this, &ObjectEdit::onEditFinished);
        connect(editStatus_, &QLineEdit::textChanged, this, &ObjectEdit::onTextChanged);
    }
}

bool ObjectEdit::confirm()
{
    return true;
}

void ObjectEdit::cancel()
{

}

bool ObjectEdit::validate()
{
    return true;
}

void ObjectEdit::enableConfirm(bool enabled)
{
    buttonConfirm_->setEnabled(enabled);
}

void ObjectEdit::enabledCancel(bool enabled)
{
    buttonCancel_->setEnabled(enabled);
}

void ObjectEdit::setStatus(const QString &text)
{
    if (text.isEmpty()) {
        editStatus_->clear();
        editStatus_->clearFocus();
    } else {
        editStatus_->setText(text);
    }
}

ICDMetaData *ObjectEdit::data()
{
    return data_.get();
}

ICDMetaData *ObjectEdit::oldData()
{
    return oldData_.get();
}

bool ObjectEdit::setExtData(const _UIData &data, bool base)
{
    if (!data.data) {
        return false;
    }

    isBaseData_ = base;

    if (!base) {
        ICDMetaData::smtMeta &meta = *reinterpret_cast<ICDMetaData::smtMeta*>(data.data);
        if (!meta) {
            return false;
        }
        oldData_ = meta;
        data_ = oldData_->clone();
    }

    setProperty("option", data.type);

    enableCommit(false);
    enableConnect(false);
    setStatus(QString());

    if (!_init()) {
        enableConnect(true);
        return false;
    }

    enableConnect(true);

    if (data.type == GlobalDefine::optNew) {
        enableConfirm(true);
    }

    show();

    return true;
}

LimitTextEdit *ObjectEdit::editDesc() const
{
    return editDesc_;
}

QString ObjectEdit::name() const
{
    return QString::fromStdString(data_->name());
}

void ObjectEdit::setName(const QString &text)
{
    data_->setName(text.toStdString());
}

QString ObjectEdit::mark() const
{
    return QString::fromStdString(data_->proCode());
}

void ObjectEdit::setMark(const QString &text)
{
    data_->setProCode(text.toStdString());
}

QString ObjectEdit::desc() const
{
    return QString::fromStdString(data_->describe());
}

void ObjectEdit::setDesc(const QString &text)
{
    data_->setDescribe(text.toStdString());
}

bool ObjectEdit::_init()
{
    editName_->clearFocus();
    editName_->setProperty("highlight", false);
    editName_->setText(name());

    editMark_->clearFocus();
    editMark_->setProperty("highlight", false);
    editMark_->setText(mark());

    editDesc_->setText(desc());

    return init();
}

bool ObjectEdit::_validate()
{
    // name
    if (name().isEmpty()) {
        editName_->setFocus();
        editName_->setProperty("highlight", true);
        setStatus(tr("Name cannot be empty!"));
        return false;
    } else {
        QString section = "name";
        QMap<QString, QString> existed;
        QVariantList args;
        args.append(qVariantFromValue(static_cast<void*>(&existed)));
        args.append(qVariantFromValue(static_cast<void*>(&section)));
        jnotify->send("edit.queryExistedData", args);
        if (existed.contains(name())) {
            editName_->setFocus();
            editName_->setProperty("highlight", true);
            setStatus(tr("Already exists the same name \"%1\"").arg(name()));
            return false;
        } else {
            editName_->setProperty("highlight", false);
        }
    }

    // mark
    const QString mark = this->mark();
    if (mark.isEmpty()) {
        editMark_->setFocus();
        editMark_->setProperty("highlight", true);
        setStatus(tr("Identity cannot be empty!"));
        return false;
    } else {
        QString section = "code";
        QMap<QString, QString> existed;
        QVariantList args;
        args.append(qVariantFromValue(static_cast<void*>(&existed)));
        args.append(qVariantFromValue(static_cast<void*>(&section)));
        jnotify->send("edit.queryExistedData", args);
        if (existed.contains(mark)) {
            editMark_->setFocus();
            editMark_->setProperty("highlight", true);
            setStatus(tr("Already exists the same mark \"%1\"").arg(mark));
            return false;
        } else {
            editMark_->setProperty("highlight", false);
        }
    }

    // anothers
    if (!validate()) {
        return false;
    }
    //
    if (data_) {
        //
        int lengthCheck = 0;
        QVariantList args;
        args.append(qVariantFromValue(static_cast<void*>(&lengthCheck)));
        QString command("lengthCheck");
        args.append(qVariantFromValue(static_cast<void*>(&command)));
        jnotify->send("edit.queryTableInformation", args);
        if (0 == lengthCheck) {
            return true;    // 不进行长度校验
        }

        const bool newItem = (property("option").toInt() == GlobalDefine::optNew);
        // 长度
        int remains = 0;
        int offset = 0;
        // 剩余可用长度
        args.clear();
        args.append(qVariantFromValue(static_cast<void*>(&remains)));
        command = "remains";
        args.append(qVariantFromValue(static_cast<void*>(&command)));
        jnotify->send("edit.queryTableInformation", args);

        // 长度偏移量
        args.clear();
        args.append(qVariantFromValue(static_cast<void*>(&data_)));
        args.append(qVariantFromValue(static_cast<void*>(&offset)));
        jnotify->send("edit.queryLengthOffset", args);
        if (remains >= offset) {
            if (newItem && remains < 0) {   // 如果是新增，需要判定剩余长度
                setStatus(tr("Length of planned data is overflow!"));
                return false;
            }
        } else {
            setStatus(tr("Length of planned data is overflow!"));
            return false;
        }
    }

    return true;
}
