#ifndef OBJECT_EDIT_H
#define OBJECT_EDIT_H

#include <QWidget>
#include "main_global.h"

namespace Icd {
class Object;
typedef std::shared_ptr<Object> ObjectPtr;
}

class QFormLayout;
class QLineEdit;
class QPlainTextEdit;

namespace Edit {

// class ObjectEdit

class ObjectEdit : public QWidget
{
    Q_OBJECT
public:
    explicit ObjectEdit(const Icd::ObjectPtr &object, QWidget *parent = nullptr);
    virtual ~ObjectEdit();

    static ObjectEdit *create(const Icd::ObjectPtr &object);

    Icd::ObjectPtr object() const;

signals:
    void contentChanged(const QString &name);

public slots:
    virtual void updateContent(const QString &name);

protected:
    void addRow(const QString &labelText, QWidget *field);
    void addRow(const QString &labelText, QLayout *field);

private:
    Icd::ObjectPtr object_;
    QFormLayout *formLayout_;
    QLineEdit *editName_;
    QLineEdit *editMark_;
    QPlainTextEdit *editDesc_;
};

}

#endif // OBJECT_EDIT_H
