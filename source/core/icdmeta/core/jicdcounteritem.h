#ifndef JICDCOUNTERITEM_H
#define JICDCOUNTERITEM_H

#include "jicditem.h"

namespace Icd {
class CounterItem;
typedef JHandlePtr<CounterItem> CounterItemPtr;
}

namespace icdmeta {

class JIcdCounterItemPrivate;

class ICDMETA_EXPORT JIcdCounterItem : public JIcdItem
{
    Q_OBJECT
    Q_PROPERTY(uchar value READ value NOTIFY valueChanged)
public:
    explicit JIcdCounterItem(const Icd::CounterItemPtr &data, QObject *parent = 0);
    ~JIcdCounterItem();

    static void registerQmlType();
    Icd::CounterItemPtr metaData() const;

    uchar value() const;
    QString text() const;
    QString valueString() const;
    QString fullValue() const;

    Q_INVOKABLE QString dataString() const;

    Q_INVOKABLE QString counterTypeString() const;
    static QString counterTypeString(IcdCore::CounterType type);
    static IcdCore::CounterType stringCounterType(const QString &str);

signals:
    void valueChanged();

public slots:

private:
    Q_DISABLE_COPY(JIcdCounterItem)
    J_DECLARE_PRIVATE(JIcdCounterItem)
};

} // end of namespace icdmeta

#endif // JICDCOUNTERITEM_H
