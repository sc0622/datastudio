#ifndef JICDARRAYITEM_H
#define JICDARRAYITEM_H

#include "jicditem.h"

namespace Icd {
class ArrayItem;
typedef std::shared_ptr<ArrayItem> ArrayItemPtr;
}

namespace icdmeta {

class JIcdArrayItemPrivate;

class ICDMETA_EXPORT JIcdArrayItem : public JIcdItem
{
    Q_OBJECT
    Q_PROPERTY(IcdCore::ArrayType arrayType READ arrayType NOTIFY arrayTypeChanged)
    Q_PROPERTY(int count READ count NOTIFY countChanged)
public:
    explicit JIcdArrayItem(const Icd::ArrayItemPtr &data, QObject *parent = nullptr);
    ~JIcdArrayItem();

    static void registerQmlType();
    Icd::ArrayItemPtr metaData() const;

    IcdCore::ArrayType arrayType() const;
    int count() const;

    QString typeName() const override;

    QString text() const override;
    QString valueString() const override;
    QString fullValue() const override;
    QString dataString() const override;

signals:
    void arrayTypeChanged();
    void countChanged();

public slots:

private:
    Q_DISABLE_COPY(JIcdArrayItem)
    J_DECLARE_PRIVATE(JIcdArrayItem)
};

} // end of namespace icdmeta

#endif // JICDARRAYITEM_H
