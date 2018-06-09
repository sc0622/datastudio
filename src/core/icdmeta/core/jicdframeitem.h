#ifndef JICDFRAMEITEM_H
#define JICDFRAMEITEM_H

#include "jicditem.h"

namespace Icd {
class FrameItem;
typedef std::shared_ptr<FrameItem> FrameItemPtr;
}

namespace icdmeta {

class JIcdFrameItemPrivate;
class JIcdTable;

class ICDMETA_EXPORT JIcdFrameItem : public JIcdItem
{
    Q_OBJECT
    Q_PROPERTY(int count READ count NOTIFY countChanged)
    Q_PROPERTY(int sequenceCount READ sequenceCount NOTIFY sequenceCountChanged)
    Q_PROPERTY(QQmlListProperty<icdmeta::JIcdTable> tables READ tables NOTIFY tablesChanged)
public:
    explicit JIcdFrameItem(const Icd::FrameItemPtr &data, QObject *parent = nullptr);
    ~JIcdFrameItem();

    static void registerQmlType();
    Icd::FrameItemPtr metaData() const;

    int count() const;
    int sequenceCount() const;
    int childCount() const override;
    QQmlListProperty<icdmeta::JIcdTable> tables();

    QString text() const override;
    QString typeName() const override;
    QString valueString() const override;
    QString dataString() const override;

    void updateData() override;
    void resetData() override;
    void clearData() override;

    Q_INVOKABLE icdmeta::JIcdTable *itemAt(int index) const;
    Q_INVOKABLE icdmeta::JIcdObject *itemByMark(const QString &mark, bool deep = true) const;
    Q_INVOKABLE icdmeta::JIcdTable *tableByMark(const QString &mark, bool deep = true) const;
    Q_INVOKABLE icdmeta::JIcdObject *itemByDomain(
            const QString &domain, int domainType = IcdCore::DomainId) const;
    Q_INVOKABLE icdmeta::JIcdTable *tableByDomain(
            const QString &domain, int domainType = IcdCore::DomainId) const;

signals:
    void countChanged();
    void sequenceCountChanged();
    void tablesChanged();

public slots:

private:
    Q_DISABLE_COPY(JIcdFrameItem)
    J_DECLARE_PRIVATE(JIcdFrameItem)
};

} // end of namespace icdmeta

#endif // JICDFRAMEITEM_H
