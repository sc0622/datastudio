#ifndef JICDBITITEM_H
#define JICDBITITEM_H

#include "jicditem.h"

namespace Icd {
class BitItem;
typedef std::shared_ptr<BitItem> BitItemPtr;
}

namespace icdmeta {

class JIcdBitItemPrivate;

class ICDMETA_EXPORT JIcdBitItem : public JIcdItem
{
    Q_OBJECT
    Q_PROPERTY(int bitStart READ bitStart NOTIFY bitStartChanged)
    Q_PROPERTY(int bitCount READ bitCount NOTIFY bitCountChanged)
    Q_PROPERTY(int typeSize READ typeSize NOTIFY typeSizeChanged)
    Q_PROPERTY(int specCount READ specCount NOTIFY specCountChanged)
    Q_PROPERTY(QString currentSpec READ currentSpec NOTIFY currentSpecChanged)
public:
    explicit JIcdBitItem(const Icd::BitItemPtr &data, QObject *parent = nullptr);
    ~JIcdBitItem();

    static void registerQmlType();
    Icd::BitItemPtr metaData() const;

    int bitStart() const;
    int bitCount() const;
    int typeSize() const;
    int specCount() const;
    QString currentSpec() const;

    QString text() const override;
    QString valueString() const override;
    QString fullValue() const override;
    QString typeName() const override;

    Q_INVOKABLE bool testBit(int offset) const;
    Q_INVOKABLE quint64 mask() const;
    Q_INVOKABLE QString specAt(quint64 key) const;
    Q_INVOKABLE QString nameAt(int offset) const;
    Q_INVOKABLE QString descAt(int offset) const;

signals:
    void bitStartChanged();
    void bitCountChanged();
    void typeSizeChanged();
    void specCountChanged();
    void currentSpecChanged(const QString &spec);

public slots:

private:
    Q_DISABLE_COPY(JIcdBitItem)
    J_DECLARE_PRIVATE(JIcdBitItem)
};

} // end of namespace icdmeta

#endif // JICDBITITEM_H
