#ifndef JICDHEADERITEM_H
#define JICDHEADERITEM_H

#include "jicditem.h"

namespace Icd {
class HeaderItem;
typedef std::shared_ptr<HeaderItem> HeaderItemPtr;
}

namespace icdmeta {

class JIcdHeaderItemPrivate;

class ICDMETA_EXPORT JIcdHeaderItem : public JIcdItem
{
    Q_OBJECT
    Q_PROPERTY(uchar value READ value NOTIFY valueChanged)
public:
    explicit JIcdHeaderItem(const Icd::HeaderItemPtr &data, QObject *parent = nullptr);
    ~JIcdHeaderItem() override;

    static void registerQmlType();
    Icd::HeaderItemPtr metaData() const;

    QString typeName() const override;

    uchar value() const;
    QString text() const override;
    QString valueString() const override;
    QString fullValue() const override;
    QString dataString() const override;

signals:
    void valueChanged();

public slots:

private:
    Q_DISABLE_COPY(JIcdHeaderItem)
    J_DECLARE_PRIVATE(JIcdHeaderItem)
};

} // end of namespace icdmeta

#endif // JICDHEADERITEM_H
