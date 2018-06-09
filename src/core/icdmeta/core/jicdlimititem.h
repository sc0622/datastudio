#ifndef JICDLIMITITEM_H
#define JICDLIMITITEM_H

#include <QObject>
#include "../icdmeta_global.h"

namespace Icd {
class LimitItem;
typedef std::shared_ptr<LimitItem> LimitItemPtr;
}

namespace icdmeta {

class JIcdLimitItemPrivate;

class ICDMETA_EXPORT JIcdLimitItem : public QObject
{
    Q_OBJECT
    Q_PROPERTY(qreal minimum READ minimum NOTIFY minimumChanged)
    Q_PROPERTY(qreal maximum READ maximum NOTIFY maximumChanged)
    Q_PROPERTY(bool leftInf READ leftInf  NOTIFY leftInfChanged)
    Q_PROPERTY(bool rightInf READ rightInf NOTIFY rightInfChanged)
public:
    explicit JIcdLimitItem(const Icd::LimitItemPtr &data, QObject *parent = nullptr);
    ~JIcdLimitItem();

    static void registerQmlType();
    Icd::LimitItemPtr metaData() const;

    qreal minimum() const;
    qreal maximum() const;
    bool leftInf() const;
    bool rightInf() const;

    Q_INVOKABLE QString toString() const;

signals:
    void minimumChanged();
    void maximumChanged();
    void leftInfChanged();
    void rightInfChanged();

public slots:

private:
    Q_DISABLE_COPY(JIcdLimitItem)
    J_DECLARE_PRIVATE(JIcdLimitItem)
};

} // end of namespace icdmeta

#endif // JICDLIMITITEM_H
