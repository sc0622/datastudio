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
public:
    explicit JIcdArrayItem(const Icd::ArrayItemPtr &data, QObject *parent = nullptr);
    ~JIcdArrayItem();

    static void registerQmlType();
    Icd::ArrayItemPtr metaData() const;

    QString typeName() const override;

    QString text() const override;
    QString valueString() const override;
    QString fullValue() const override;
    QString dataString() const override;

signals:

public slots:

private:
    Q_DISABLE_COPY(JIcdArrayItem)
    J_DECLARE_PRIVATE(JIcdArrayItem)
};

} // end of namespace icdmeta

#endif // JICDARRAYITEM_H
