#include "precomp.h"
#include "ObjectEdit.h"
#include "VehicleEdit.h"
#include "SystemEdit.h"
#include "TableEdit.h"
#include "ItemEdit.h"

namespace Edit {

// class ObjectEdit

ObjectEdit::ObjectEdit(const Icd::ObjectPtr &object, QWidget *parent)
    : QWidget(parent)
    , object_(object)
    , blocking_(false)
{
    layoutMain_ = new QVBoxLayout(this);
    layoutMain_->setContentsMargins(0, 0, 0, 0);
    layoutMain_->setSpacing(3);

    splitter_ = new JSplitter({1, 2}, Qt::Vertical, this);
    splitter_->setHandleWidth(3);
    layoutMain_->addWidget(splitter_);

    QGroupBox *groupBase = new QGroupBox(tr("Base informations"), this);
    splitter_->addWidget(groupBase);

    formLayout_ = new QFormLayout(groupBase);
    formLayout_->setContentsMargins(3, 0, 3, 3);
    formLayout_->setLabelAlignment(Qt::AlignRight);

    editName_ = new QLineEdit(this);
    formLayout_->addRow("<font color=red>*</font>" + tr("Name:"), editName_);

    editMark_ = new QLineEdit(this);
    editMark_->setValidator(new QRegExpValidator(QRegExp("^[a-zA-Z_][a-zA-Z0-9_]{0,256}$")));
    formLayout_->addRow(tr("Mark:"), editMark_);

    editDesc_ = new QPlainTextEdit(this);
    formLayout_->addRow(tr("Describe:"), editDesc_);

    connect(editName_, &QLineEdit::textChanged, this, [=](){
        if (blocking()) {
            return;
        }
        emit contentChanged();
    });
    connect(editMark_, &QLineEdit::textChanged, this, [=](){
        if (blocking()) {
            return;
        }
        emit contentChanged();
    });
    connect(editDesc_, &QPlainTextEdit::textChanged, this, [=](){
        if (blocking()) {
            return;
        }
        emit contentChanged();
    });
}

ObjectEdit::~ObjectEdit()
{

}

Icd::ObjectPtr ObjectEdit::object() const
{
    return object_;
}

int ObjectEdit::itemType() const
{
    return Icd::ItemInvalid;
}

ObjectEdit *ObjectEdit::create(const Icd::ObjectPtr &object)
{
    if (!object) {
        return nullptr;
    }

    switch (object->objectType()) {
    case Icd::ObjectVehicle:
        return new VehicleEdit(JHandlePtrCast<Icd::Vehicle>(object));
    case Icd::ObjectSystem:
        return new SystemEdit(JHandlePtrCast<Icd::System>(object));
    case Icd::ObjectTable:
        return new TableEdit(JHandlePtrCast<Icd::Table>(object));
    case Icd::ObjectItem:
        return ItemEdit::create(JHandlePtrCast<Icd::Item>(object));
    default:
        break;
    }

    return nullptr;
}

bool ObjectEdit::init()
{
    ObjectEdit::restoreContent(false);

    return true;
}

void ObjectEdit::lock()
{
    blocking_ = true;
}

void ObjectEdit::unlock()
{
    blocking_ = false;
}

bool ObjectEdit::blocking() const
{
    return blocking_;
}

bool ObjectEdit::trySaveContent()
{
    if (!object_) {
        return false;
    }

    if (!validate()) {
        return false;
    }

    saveContent();

    return true;
}

void ObjectEdit::restoreContent(bool recursive)
{
    Q_UNUSED(recursive);
    if (!object_) {
        return;
    }

    lock();

    const Icd::ObjectPtr object = this->object();
    if (object) {
        // name
        editName_->setText(QString::fromStdString(object->name()));
        // mark
        editMark_->setText(QString::fromStdString(object->mark()));
        // desc
        editDesc_->setPlainText(QString::fromStdString(object->desc()));
    }

    unlock();
}

const QStringList &ObjectEdit::primaryModified() const
{
    return primaryModified_;
}

bool ObjectEdit::existsPrimaryModified(const QString &key) const
{
    return primaryModified_.contains(key);
}

void ObjectEdit::insertRow(int index, const QString &labelText, QWidget *field)
{
    formLayout_->insertRow(index, labelText, field);
}

void ObjectEdit::addRow(const QString &labelText, QWidget *field)
{
    formLayout_->insertRow(formLayout_->rowCount() - 1, labelText, field);
}

void ObjectEdit::addRow(const QString &labelText, QLayout *field)
{
    formLayout_->insertRow(formLayout_->rowCount() - 1, labelText, field);
}

void ObjectEdit::appendRow(const QString &labelText, QWidget *field)
{
    formLayout_->addRow(labelText, field);
}

void ObjectEdit::appendRow(const QString &labelText, QLayout *field)
{
    formLayout_->addRow(labelText, field);
}

void ObjectEdit::addWidget(QWidget *widget)
{
    splitter_->addWidget(widget);
}

bool ObjectEdit::validate()
{
    // name
    if (editName_->text().trimmed().isEmpty()) {
        editName_->setFocus();
        return false;
    }

    return true;
}

void ObjectEdit::saveContent()
{
    // name
    object_->setName(editName_->text().trimmed().toStdString());
    // mark
    object_->setMark(editMark_->text().trimmed().toStdString());
    // desc
    object_->setDesc(editDesc_->toPlainText().trimmed().toStdString());
}

void ObjectEdit::addPrimaryModified(const QString &key)
{
    if (!primaryModified_.contains(key)) {
        primaryModified_.append(key);
    }
}

void ObjectEdit::removePrimaryModified(const QString &key)
{
    primaryModified_.removeAll(key);
}

void ObjectEdit::setMarkReadOnly(bool readOnly)
{
    editMark_->setReadOnly(readOnly);
}

void ObjectEdit::setMarkValidator(const QValidator *validator)
{
    editMark_->setValidator(validator);
}

void ObjectEdit::setMark(const QString &text)
{
    editMark_->setText(text);
}

}
