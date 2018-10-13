#ifndef JICDCOUNTERITEM_H
#define JICDCOUNTERITEM_H

#include "jicditem.h"

namespace Icd {
class CounterItem;
typedef std::shared_ptr<CounterItem> CounterItemPtr;
}

namespace icdmeta {

class JIcdCounterItemPrivate;

class ICDMETA_EXPORT JIcdCounterItem : public JIcdItem
{
    Q_OBJECT
    Q_PROPERTY(IcdCore::CounterType counterType READ counterType NOTIFY counterTypeChanged)
    Q_PROPERTY(uchar value READ value NOTIFY valueChanged)
public:
    explicit JIcdCounterItem(const Icd::CounterItemPtr &data, QObject *parent = nullptr);
    ~JIcdCounterItem() override;

    static void registerQmlType();
    Icd::CounterItemPtr metaData() const;

    IcdCore::CounterType counterType() const;
    Q_INVOKABLE QString counterTypeString() const;
    static QString counterTypeString(IcdCore::CounterType type);
    static IcdCore::CounterType stringCounterType(const QString &str);

    uchar value() const;

    QString text() const override;
    QString valueString() const override;
    QString fullValue() const override;
    QString dataString() const override;

signals:
    void counterTypeChanged();
    void valueChanged();

public slots:

private:
    Q_DISABLE_COPY(JIcdCounterItem)
    J_DECLARE_PRIVATE(JIcdCounterItem)
};

} // end of namespace icdmeta

#endif // JICDCOUNTERITEM_H
