#ifndef JICDCOMPLEXITEM_H
#define JICDCOMPLEXITEM_H

#include "jicditem.h"

namespace Icd {
class ComplexItem;
typedef std::shared_ptr<ComplexItem> ComplexItemPtr;
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
    explicit JIcdComplexItem(const Icd::ComplexItemPtr &data, QObject *parent = nullptr);
    ~JIcdComplexItem() override;

    static void registerQmlType();
    Icd::ComplexItemPtr metaData() const;

    int childCount() const override;
    icdmeta::JIcdTable *table() const;

    QString text() const override;
    QString valueString() const override;
    QString typeName() const override;

    void updateData() override;
    void resetData() override;
    void clearData() override;

    Q_INVOKABLE icdmeta::JIcdObject *itemByMark(const QString &mark, bool deep = true) const;
    Q_INVOKABLE icdmeta::JIcdTable *tableByMark(const QString &mark, bool deep = true) const;
    Q_INVOKABLE icdmeta::JIcdObject *itemByDomain(
            const QString &domain, int domainType = IcdCore::DomainId) const;
    Q_INVOKABLE icdmeta::JIcdTable *tableByDomain(
            const QString &domain, int domainType = IcdCore::DomainId) const;

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
