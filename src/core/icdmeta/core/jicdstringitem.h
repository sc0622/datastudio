#ifndef JICDSTRINGITEM_H
#define JICDSTRINGITEM_H

#include "jicditem.h"

namespace Icd {
class StringItem;
typedef std::shared_ptr<StringItem> StringItemPtr;
}

namespace icdmeta {

class JIcdStringItemPrivate;

class ICDMETA_EXPORT JIcdStringItem : public JIcdItem
{
    Q_OBJECT
public:
    explicit JIcdStringItem(const Icd::StringItemPtr &data, QObject *parent = nullptr);
    ~JIcdStringItem();

    static void registerQmlType();
    Icd::StringItemPtr metaData() const;

    QString typeName() const override;

    QString text() const override;
    QString valueString() const override;
    QString fullValue() const override;
    QString dataString() const override;

signals:

public slots:

private:
    Q_DISABLE_COPY(JIcdStringItem)
    J_DECLARE_PRIVATE(JIcdStringItem)
};

} // end of namespace icdmeta

#endif // JICDSTRINGITEM_H
