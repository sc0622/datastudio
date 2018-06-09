#ifndef JICDOBJECT_H
#define JICDOBJECT_H

#include <QObject>
#include "../icdmeta_global.h"

namespace Icd {
template<typename T> class std::shared_ptr;
class Object;
typedef std::shared_ptr<Object> ObjectPtr;
}

namespace icdmeta {

class JIcdObjectPrivate;

class ICDMETA_EXPORT JIcdObject : public QObject
{
    Q_OBJECT
    Q_PROPERTY(int objectType READ objectType NOTIFY objectTypeChanged)
    Q_PROPERTY(QString id READ id WRITE setId NOTIFY idChanged)
    Q_PROPERTY(QString name READ name WRITE setName NOTIFY nameChanged)
    Q_PROPERTY(QString mark READ mark WRITE setMark NOTIFY markChanged)
    Q_PROPERTY(QString desc READ desc WRITE setDesc NOTIFY descChanged)
    Q_PROPERTY(int childCount READ childCount NOTIFY childCountChanged)
    Q_PROPERTY(QString text READ text NOTIFY textChanged)
    Q_PROPERTY(QString valueString READ valueString NOTIFY valueStringChanged)
    Q_PROPERTY(bool checked READ isChecked WRITE setChecked NOTIFY checkedChanged)
public:
    explicit JIcdObject(const Icd::ObjectPtr &data, QObject *parent = 0);

    static void registerQmlType();

    Icd::ObjectPtr metaData() const;

    int objectType() const;
    Q_INVOKABLE QString objectTypeString() const;

    QString id() const;
    QString name() const;
    QString mark() const;
    QString desc() const;

    bool isChecked() const;
    Q_INVOKABLE bool isPrivateMark() const;
    Q_INVOKABLE QString domainOfType(int domainType) const;

    virtual int childCount() const;
    virtual QString text() const;
    virtual QString valueString() const;
    Q_INVOKABLE virtual void updateData();
    Q_INVOKABLE virtual void resetData();
    Q_INVOKABLE virtual void clearData();

signals:
    void objectTypeChanged(int value);
    void idChanged(const QString &value);
    void nameChanged(const QString &value);
    void markChanged(const QString &value);
    void descChanged(const QString &value);
    void childCountChanged(int value);
    void textChanged();
    void valueStringChanged();
    void checkedChanged(bool value);

public slots:
    void setId(const QString &value);
    void setName(const QString &value);
    void setMark(const QString &value);
    void setDesc(const QString &value);
    void setChecked(bool value);

private:
    Q_DISABLE_COPY(JIcdObject)
    J_DECLARE_PRIVATE(JIcdObject)
};

} // end of namespace icdmeta

#endif // JICDOBJECT_H
