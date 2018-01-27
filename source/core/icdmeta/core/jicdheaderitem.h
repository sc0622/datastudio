#ifndef JICDHEADERITEM_H
#define JICDHEADERITEM_H

#include "jicditem.h"

namespace Icd {
class HeaderItem;
typedef JHandlePtr<HeaderItem> HeaderItemPtr;
}

namespace icdmeta {

class JIcdHeaderItemPrivate;

class ICDMETA_EXPORT JIcdHeaderItem : public JIcdItem
{
    Q_OBJECT
    Q_PROPERTY(QString dataString READ dataString NOTIFY dataStringChanged)
    Q_PROPERTY(QString typeName READ typeName NOTIFY typeNameChanged)
    Q_PROPERTY(uchar value READ value NOTIFY valueChanged)
public:
    explicit JIcdHeaderItem(const Icd::HeaderItemPtr &data, QObject *parent = 0);
    ~JIcdHeaderItem();

    static void registerQmlType();
    Icd::HeaderItemPtr metaData() const;

    QString dataString() const;
    QString typeName() const;
    uchar value() const;

    QString text() const;
    QString valueString() const;
    QString fullValue() const;

signals:
    void dataStringChanged();
    void typeNameChanged();
    void valueChanged();

public slots:

private:
    Q_DISABLE_COPY(JIcdHeaderItem)
    J_DECLARE_PRIVATE(JIcdHeaderItem)
};

} // end of namespace icdmeta

#endif // JICDHEADERITEM_H
