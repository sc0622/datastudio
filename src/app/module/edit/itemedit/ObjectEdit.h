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
class QValidator;

namespace Edit {

// class ObjectEdit

class ObjectEdit : public QWidget
{
    Q_OBJECT
public:
    explicit ObjectEdit(const Icd::ObjectPtr &object, QWidget *parent = nullptr);
    virtual ~ObjectEdit() override;

    Icd::ObjectPtr object() const;
    virtual int itemType() const;

    static ObjectEdit *create(const Icd::ObjectPtr &object);

    virtual bool init();

    void lock();
    void unlock();
    bool blocking() const;

    bool trySaveContent();
    virtual void restoreContent(bool recursive = true);

    const QStringList &primaryModified() const;
    bool existsPrimaryModified(const QString &key) const;

    void focusName();

signals:
    void contentChanged(const QString &key = QString(), const QVariant &value = QVariant());
    void itemTypeChanged(int type);

public slots:

protected:
    void insertRow(int index, const QString &labelText, QWidget *field);
    virtual void addRow(const QString &labelText, QWidget *field);
    virtual void addRow(const QString &labelText, QLayout *field);
    void appendRow(const QString &labelText, QWidget *field);
    void appendRow(const QString &labelText, QLayout *field);
    void addWidget(QWidget *widget);

    virtual bool validate() const;
    virtual bool validateMark() const;
    virtual void saveContent();
    void addPrimaryModified(const QString &key);
    void removePrimaryModified(const QString &key);

    void setMarkReadOnly(bool readOnly);
    void setMarkValidator(const QValidator *validator);
    void setMark(const QString &text);

private:
    Icd::ObjectPtr object_;
    QVBoxLayout *layoutMain_;
    JSplitter *splitter_;
    QFormLayout *formLayout_;
    QLineEdit *editName_;
    QLineEdit *editMark_;
    QPlainTextEdit *editDesc_;
    QStringList primaryModified_;
    bool blocking_;
};

}

#endif // OBJECT_EDIT_H
