#ifndef JICDCHECKITEM_H
#define JICDCHECKITEM_H

#include "jicditem.h"

namespace Icd {
class CheckItem;
typedef JHandlePtr<CheckItem> CheckItemPtr;
}

namespace icdmeta {

class JIcdCheckItemPrivate;

class ICDMETA_EXPORT JIcdCheckItem : public JIcdItem
{
    Q_OBJECT
    Q_PROPERTY(IcdCore::CheckType checkType READ checkType NOTIFY checkTypeChanged)
    Q_PROPERTY(int startPos READ startPos NOTIFY startPosChanged)
    Q_PROPERTY(int endPos READ endPos NOTIFY endPosChanged)
    Q_PROPERTY(int checkLength READ checkLength  NOTIFY checkLengthChanged)
public:
    explicit JIcdCheckItem(const Icd::CheckItemPtr &data, QObject *parent = 0);
    ~JIcdCheckItem();

    static void registerQmlType();
    Icd::CheckItemPtr metaData() const;

    IcdCore::CheckType checkType() const;
    int startPos() const;
    int endPos() const;
    int checkLength() const;

    Q_INVOKABLE bool isValid() const;
    Q_INVOKABLE QString typeName() const;

    Q_INVOKABLE QString checkTypeString() const;
    static QString checkTypeString(IcdCore::CheckType type);
    static IcdCore::CheckType stringCheckType(const QString &str);

    QString text() const;
    QString valueString() const;
    QString fullValue() const;

signals:
    void checkTypeChanged();
    void startPosChanged();
    void endPosChanged();
    void checkLengthChanged();

public slots:

private:
    Q_DISABLE_COPY(JIcdCheckItem)
    J_DECLARE_PRIVATE(JIcdCheckItem)
};

} // end of namespace icdmeta

#endif // JICDCHECKITEM_H
