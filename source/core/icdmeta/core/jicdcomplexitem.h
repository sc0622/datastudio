#ifndef JICDCOMPLEXITEM_H
#define JICDCOMPLEXITEM_H

#include "jicditem.h"

namespace Icd {
class ComplexItem;
typedef JHandlePtr<ComplexItem> ComplexItemPtr;
}

namespace icdmeta {

class JIcdComplexItemPrivate;
class JIcdTable;

class ICDMETA_EXPORT JIcdComplexItem : public JIcdItem
{
    Q_OBJECT
    Q_PROPERTY(int childCount READ childCount NOTIFY childCountChanged)
    Q_PROPERTY(icdmeta::JIcdTable *table READ table NOTIFY tableChanged)
public:
    explicit JIcdComplexItem(const Icd::ComplexItemPtr &data, QObject *parent = 0);
    ~JIcdComplexItem();

    static void registerQmlType();
    Icd::ComplexItemPtr metaData() const;

    int childCount() const;
    icdmeta::JIcdTable *table() const;

    QString text() const;
    QString valueString() const;
    Q_INVOKABLE QString typeName() const;
    Q_INVOKABLE icdmeta::JIcdObject *itemByMark(const QString &mark, bool deep = true) const;
    Q_INVOKABLE icdmeta::JIcdTable *tableByMark(const QString &mark, bool deep = true) const;
    Q_INVOKABLE icdmeta::JIcdObject *itemByDomain(
            const QString &domain, int domainType = IcdCore::DomainId) const;
    Q_INVOKABLE icdmeta::JIcdTable *tableByDomain(
            const QString &domain, int domainType = IcdCore::DomainId) const;
    Q_INVOKABLE virtual void updateData();
    Q_INVOKABLE virtual void reset();

signals:
    void childCountChanged();
    void tableChanged();

public slots:

private:
    Q_DISABLE_COPY(JIcdComplexItem)
    J_DECLARE_PRIVATE(JIcdComplexItem)
};

} // end of namespace icdmeta

#endif // JICDCOMPLEXITEM_H
