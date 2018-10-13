#ifndef JICDDATETIMEITEM_H
#define JICDDATETIMEITEM_H

#include "jicditem.h"

namespace Icd {
class DateTimeItem;
typedef std::shared_ptr<DateTimeItem> DateTimeItemPtr;
}

namespace icdmeta {

class JIcdDateTimeItemPrivate;

class ICDMETA_EXPORT JIcdDateTimeItem : public JIcdItem
{
    Q_OBJECT
public:
    explicit JIcdDateTimeItem(const Icd::DateTimeItemPtr &data, QObject *parent = nullptr);
    ~JIcdDateTimeItem() override;

    static void registerQmlType();
    Icd::DateTimeItemPtr metaData() const;

    QString typeName() const override;

    QString text() const override;
    QString valueString() const override;
    QString fullValue() const override;
    QString dataString() const override;

signals:

public slots:

private:
    Q_DISABLE_COPY(JIcdDateTimeItem)
    J_DECLARE_PRIVATE(JIcdDateTimeItem)
};

} // end of namespace icdmeta

#endif // JICDDATETIMEITEM_H
