#ifndef JICDFRAMEITEM_H
#define JICDFRAMEITEM_H

#include "jicditem.h"

namespace Icd {
class FrameItem;
typedef JHandlePtr<FrameItem> FrameItemPtr;
}

namespace icdmeta {

class JIcdFrameItemPrivate;
class JIcdTable;

class ICDMETA_EXPORT JIcdFrameItem : public JIcdItem
{
    Q_OBJECT
    Q_PROPERTY(int count READ count NOTIFY countChanged)
    Q_PROPERTY(int sequenceCount READ sequenceCount NOTIFY sequenceCountChanged)
    Q_PROPERTY(QString dataString READ dataString NOTIFY dataStringChanged)
    Q_PROPERTY(QString typeName READ typeName NOTIFY typeNameChanged)
    Q_PROPERTY(int childCount READ childCount NOTIFY childCountChanged)
    Q_PROPERTY(QQmlListProperty<icdmeta::JIcdTable> tables READ tables NOTIFY tablesChanged)
public:
    explicit JIcdFrameItem(const Icd::FrameItemPtr &data, QObject *parent = 0);
    ~JIcdFrameItem();

    static void registerQmlType();
    Icd::FrameItemPtr metaData() const;

    int count() const;
    int sequenceCount() const;
    QString dataString() const;
    QString typeName() const;
    int childCount() const;
    QQmlListProperty<icdmeta::JIcdTable> tables();

    Q_INVOKABLE icdmeta::JIcdTable *itemAt(int index) const;
    Q_INVOKABLE icdmeta::JIcdObject *itemByMark(const QString &mark, bool deep = true) const;
    Q_INVOKABLE icdmeta::JIcdTable *tableByMark(const QString &mark, bool deep = true) const;
    Q_INVOKABLE icdmeta::JIcdObject *itemByDomain(
            const QString &domain, int domainType = IcdCore::DomainId) const;
    Q_INVOKABLE icdmeta::JIcdTable *tableByDomain(
            const QString &domain, int domainType = IcdCore::DomainId) const;
    QString text() const;
    QString valueString() const;
    Q_INVOKABLE virtual void updateData();
    Q_INVOKABLE virtual void reset();

signals:
    void countChanged();
    void sequenceCountChanged();
    void dataStringChanged();
    void typeNameChanged();
    void childCountChanged();
    void tablesChanged();

public slots:

private:
    Q_DISABLE_COPY(JIcdFrameItem)
    J_DECLARE_PRIVATE(JIcdFrameItem)
};

} // end of namespace icdmeta

#endif // JICDFRAMEITEM_H
