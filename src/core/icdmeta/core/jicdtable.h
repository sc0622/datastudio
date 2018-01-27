#ifndef JICDTABLE_H
#define JICDTABLE_H

#include "jicditem.h"

namespace Icd {
class Table;
typedef JHandlePtr<Table> TablePtr;
}

namespace icdmeta {

class JIcdTablePrivate;
class JIcdCounterItem;
class JIcdCheckItem;
class JIcdFrameCodeItem;

class ICDMETA_EXPORT JIcdTable : public JIcdObject
{
    Q_OBJECT
    Q_PROPERTY(int itemOffset READ itemOffset NOTIFY itemOffsetChanged)
    Q_PROPERTY(qreal bufferSize READ bufferSize NOTIFY bufferSizeChanged)
    Q_PROPERTY(qreal bufferOffset READ bufferOffset NOTIFY bufferOffsetChanged)
    Q_PROPERTY(int count READ count NOTIFY countChanged)
    Q_PROPERTY(int sequence READ sequence WRITE setSequence NOTIFY sequenceChanged)
    Q_PROPERTY(QByteArray headers READ metaHeaders NOTIFY headersChanged)
    Q_PROPERTY(icdmeta::JIcdCounterItem* counterItem READ metaCounterItem NOTIFY counterItemChanged)
    Q_PROPERTY(icdmeta::JIcdCheckItem* checkItem READ metaCheckItem NOTIFY checkItemChanged)
    Q_PROPERTY(QQmlListProperty<icdmeta::JIcdFrameCodeItem> frameCodes READ metaFrameCodes NOTIFY frameCodesChanged)
public:
    explicit JIcdTable(const Icd::TablePtr &data, QObject *parent = 0);
    ~JIcdTable();

    static void registerQmlType();
    Icd::TablePtr metaData() const;

    int itemOffset() const;
    qreal bufferSize() const;
    qreal bufferOffset() const;
    int count() const;
    int sequence() const;
    QString text() const;

    char *buffer() const;
    void setBuffer(char *buffer);

    Q_INVOKABLE icdmeta::JIcdItem *itemAt(int index) const;
    Q_INVOKABLE icdmeta::JIcdItem *itemById(const QString &id) const;
    Q_INVOKABLE icdmeta::JIcdObject *itemByMark(const QString &mark, bool deep = true) const;
    Q_INVOKABLE icdmeta::JIcdTable *tableByMark(const QString &mark, bool deep = true) const;
    Q_INVOKABLE icdmeta::JIcdObject *itemByDomain(
            const QString &domain, int domainType = IcdCore::DomainId) const;
    Q_INVOKABLE icdmeta::JIcdTable *tableByDomain(
            const QString &domain, int domainType = IcdCore::DomainId) const;

    Q_INVOKABLE void updateData();
    Q_INVOKABLE void resetData();
    Q_INVOKABLE void clearData();

    QVector<char> headers() const;
    QSharedPointer<icdmeta::JIcdCounterItem> counterItem();
    const QSharedPointer<icdmeta::JIcdCounterItem> &counterItem() const;
    bool isCheckValid() const;
    QSharedPointer<icdmeta::JIcdCheckItem> checkItem();
    const QSharedPointer<icdmeta::JIcdCheckItem> &checkItem() const;
    const QList<QSharedPointer<icdmeta::JIcdFrameCodeItem> > &frameCodes() const;
    void updateSend(bool period);
    void resetSend();
    void bindBuffer(void *buffer);
    QString typeName() const;
    QString codeName() const;

    QByteArray metaHeaders() const;
    icdmeta::JIcdCounterItem *metaCounterItem() const;
    icdmeta::JIcdCheckItem *metaCheckItem() const;
    QQmlListProperty<icdmeta::JIcdFrameCodeItem> metaFrameCodes();

signals:
    void itemOffsetChanged();
    void bufferSizeChanged();
    void bufferOffsetChanged();
    void countChanged();
    void sequenceChanged(int value);
    void headersChanged();
    void counterItemChanged();
    void checkItemChanged();
    void frameCodesChanged();

public slots:
    void setSequence(int value);

private:
    Q_DISABLE_COPY(JIcdTable)
    J_DECLARE_PRIVATE(JIcdTable)
};

} // end of namespace icdmeta

#endif // JICDTABLE_H
