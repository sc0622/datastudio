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
{
    QVBoxLayout *layoutMain = new QVBoxLayout(this);
    layoutMain->setContentsMargins(0, 0, 0, 0);

    formLayout_ = new QFormLayout();
    formLayout_->setLabelAlignment(Qt::AlignRight);
    layoutMain->addLayout(formLayout_);

    editName_ = new QLineEdit(this);
    formLayout_->addRow("<font color=red>*</font>" + tr("Name:"), editName_);

    editMark_ = new QLineEdit(this);
    formLayout_->addRow("<font color=red>*</font>" + tr("Mark:"), editMark_);

    editDesc_ = new QPlainTextEdit(this);
    formLayout_->addRow(tr("Describe:"), editDesc_);

    if (object) {
        //
        editName_->setText(QString::fromStdString(object->name()));
        editMark_->setText(QString::fromStdString(object->mark()));
        editDesc_->setPlainText(QString::fromStdString(object->desc()));
        //
        connect(editName_, &QLineEdit::textChanged, this, [=](const QString &text){
            if (object_) {
                object_->setName(text.toStdString());
                emit contentChanged("name");
            }
        });
        connect(editMark_, &QLineEdit::textChanged, this, [=](const QString &text){
            if (object_) {
                object_->setMark(text.toStdString());
                emit contentChanged("mark");
            }
        });
        connect(editDesc_, &QPlainTextEdit::textChanged, this, [=](){
            if (object_) {
                object_->setDesc(editDesc_->toPlainText().toStdString());
                emit contentChanged("desc");
            }
        });
    }
}

ObjectEdit::~ObjectEdit()
{

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

Icd::ObjectPtr ObjectEdit::object() const
{
    return object_;
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

void ObjectEdit::addRow(const QString &labelText, QWidget *field)
{
    formLayout_->insertRow(formLayout_->rowCount() - 2, labelText, field);
}

void ObjectEdit::addRow(const QString &labelText, QLayout *field)
{
    formLayout_->insertRow(formLayout_->rowCount() - 2, labelText, field);
}

}
