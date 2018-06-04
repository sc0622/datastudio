#ifndef JICDITEM_H
#define JICDITEM_H

#include "jicdobject.h"

namespace Icd {
template<typename T> class std::shared_ptr;
class Item;
typedef std::shared_ptr<Item> ItemPtr;
}

namespace icdmeta {

class JIcdItemPrivate;

class ICDMETA_EXPORT JIcdItem : public JIcdObject
{
    Q_OBJECT
    Q_PROPERTY(IcdCore::ItemType itemType READ itemType NOTIFY itemTypeChanged)
    Q_PROPERTY(int itemOffset READ itemOffset NOTIFY itemOffsetChanged)
    Q_PROPERTY(qreal bufferSize READ bufferSize NOTIFY bufferSizeChanged)
    Q_PROPERTY(qreal bufferOffset READ bufferOffset NOTIFY bufferOffsetChanged)
    Q_PROPERTY(qreal data READ data WRITE setData NOTIFY dataChanged)
    Q_PROPERTY(double defaultValue READ defaultValue WRITE setDefaultValue NOTIFY defaultValueChanged)
    Q_PROPERTY(QString fullValue READ fullValue NOTIFY fullValueChanged)
public:
    explicit JIcdItem(const Icd::ItemPtr &data, QObject *parent = 0);

    static void registerQmlType();
    Icd::ItemPtr metaData() const;

    IcdCore::ItemType itemType() const;
    int itemOffset() const;
    qreal bufferSize() const;
    qreal bufferOffset() const;
    qreal data() const;
    double defaultValue() const;
    virtual QString text() const;
    virtual QString valueString() const;
    virtual QString fullValue() const;

    Q_INVOKABLE QString typeString() const;
    Q_INVOKABLE QString dataString() const;
    Q_INVOKABLE QString typeName() const;

    static QString typeString(IcdCore::ItemType type);
    static IcdCore::ItemType stringType(const QString &str);

    static JIcdItem *create(const Icd::ItemPtr &data, QObject *parent = 0);

    Q_INVOKABLE virtual void updateData();
    Q_INVOKABLE virtual void resetData();
    Q_INVOKABLE virtual void clearData();

signals:
    void itemTypeChanged(IcdCore::ItemType value);
    void itemOffsetChanged(int value);
    void bufferSizeChanged(qreal value);
    void bufferOffsetChanged(qreal value);
    void dataChanged(qreal value);
    void defaultValueChanged(qreal value);
    void fullValueChanged();

public slots:
    void setDefaultValue(qreal value);
    void setData(qreal value);

private:
    Q_DISABLE_COPY(JIcdItem)
    J_DECLARE_PRIVATE(JIcdItem)
};

} // end of namespace icdmeta

#endif // JICDITEM_H
