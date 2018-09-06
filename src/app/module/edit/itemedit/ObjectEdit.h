#ifndef OBJECT_EDIT_H
#define OBJECT_EDIT_H

#include <QWidget>
#include "main_global.h"

namespace Icd {
class Object;
typedef std::shared_ptr<Object> ObjectPtr;
}

class JSplitter;
class QVBoxLayout;
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

    Icd::ObjectPtr object() const;

    static ObjectEdit *create(const Icd::ObjectPtr &object);

    virtual bool init();

    void lock();
    void unlock();
    bool blocking() const;

signals:
    void contentChanged(const QString &name);

public slots:
    virtual void updateContent(const QString &name);

protected:
    void insertRow(int index, const QString &labelText, QWidget *field);
    virtual void addRow(const QString &labelText, QWidget *field);
    virtual void addRow(const QString &labelText, QLayout *field);
    void appendRow(const QString &labelText, QWidget *field);
    void appendRow(const QString &labelText, QLayout *field);
    void addWidget(QWidget *widget);

    virtual bool validate();

    void setMarkReadOnly(bool readOnly);
    void setMark(const QString &text);

private:
    Icd::ObjectPtr object_;
    QVBoxLayout *layoutMain_;
    JSplitter *splitter_;
    QFormLayout *formLayout_;
    QLineEdit *editName_;
    QLineEdit *editMark_;
    QPlainTextEdit *editDesc_;
    bool blocking_;
};

}

#endif // OBJECT_EDIT_H
