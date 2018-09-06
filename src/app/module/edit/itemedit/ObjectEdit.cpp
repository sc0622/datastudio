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
    formLayout_->addRow(tr("Mark:"), editMark_);

    editDesc_ = new QPlainTextEdit(this);
    formLayout_->addRow(tr("Describe:"), editDesc_);

    if (object) {
        //
        editName_->setText(QString::fromStdString(object->name()));
        editMark_->setText(QString::fromStdString(object->mark()));
        editDesc_->setPlainText(QString::fromStdString(object->desc()));
        //
        connect(editName_, &QLineEdit::textChanged, this, [=](const QString &text){
            Q_UNUSED(text);
            if (object_) {
                emit contentChanged("name");
            }
        });
        connect(editMark_, &QLineEdit::textChanged, this, [=](const QString &text){
            Q_UNUSED(text);
            if (object_) {
                emit contentChanged("mark");
            }
        });
        connect(editDesc_, &QPlainTextEdit::textChanged, this, [=](){
            if (object_) {
                emit contentChanged("desc");
            }
        });
    }
}

ObjectEdit::~ObjectEdit()
{

}

Icd::ObjectPtr ObjectEdit::object() const
{
    return object_;
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
    lock();

    //

    unlock();

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

void ObjectEdit::updateContent(const QString &name)
{
    if (!object_) {
        return;
    }

    if (name == "name") {
        editName_->setText(QString::fromStdString(object_->name()));
    } else if (name == "mark") {
        editMark_->setText(QString::fromStdString(object_->mark()));
    } else if (name == "desc") {
        editDesc_->setPlainText(QString::fromStdString(object_->desc()));
    }
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
    return true;
}

void ObjectEdit::setMarkReadOnly(bool readOnly)
{
    editMark_->setReadOnly(readOnly);
}

void ObjectEdit::setMark(const QString &text)
{
    editMark_->setText(text);
}

}
