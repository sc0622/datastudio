#ifndef JICDNUMERICITEM_H
#define JICDNUMERICITEM_H

#include "jicditem.h"

namespace Icd {
class NumericItem;
typedef std::shared_ptr<NumericItem> NumericItemPtr;
}

namespace icdmeta {

class JIcdNumericItemPrivate;
class JIcdLimitItem;

class ICDMETA_EXPORT JIcdNumericItem : public JIcdItem
{
    Q_OBJECT
    Q_PROPERTY(IcdCore::NumericType numericType READ numericType NOTIFY numericTypeChanged)
    Q_PROPERTY(qreal scale READ scale NOTIFY scaleChanged)
    Q_PROPERTY(qreal offset READ offset NOTIFY offsetChanged)
    Q_PROPERTY(qreal decimals READ decimals NOTIFY decimalsChanged)
    Q_PROPERTY(icdmeta::JIcdLimitItem* limit READ limit NOTIFY limitChanged)
    Q_PROPERTY(QString unit READ unit NOTIFY unitChanged)
    Q_PROPERTY(int specCount READ specCount NOTIFY specCountChanged)
public:
    explicit JIcdNumericItem(const Icd::NumericItemPtr &data, QObject *parent = 0);
    ~JIcdNumericItem();

    static void registerQmlType();
    Icd::NumericItemPtr metaData() const;

    IcdCore::NumericType numericType() const;
    qreal scale() const;
    qreal offset() const;
    qreal decimals() const;
    icdmeta::JIcdLimitItem *limit() const;
    QString unit() const;
    int specCount() const;

    Q_INVOKABLE QString specAt(double key) const;
    Q_INVOKABLE qreal dataMinimum() const;
    Q_INVOKABLE qreal dataMaximum() const;
    Q_INVOKABLE qreal valueMinimum() const;
    Q_INVOKABLE qreal valueMaximum() const;

    Q_INVOKABLE double originalData() const;
    Q_INVOKABLE bool outOfLimit() const;

    QString text() const override;
    QString valueString() const override;
    QString fullValue() const override;
    QString typeName() const override;
    QString dataString() const override;

signals:
    void numericTypeChanged();
    void scaleChanged();
    void offsetChanged();
    void decimalsChanged();
    void limitChanged();
    void unitChanged();
    void specCountChanged();

public slots:

private:
    Q_DISABLE_COPY(JIcdNumericItem)
    J_DECLARE_PRIVATE(JIcdNumericItem)
};

} // end of namespace icdmeta

#endif // JICDNUMERICITEM_H
