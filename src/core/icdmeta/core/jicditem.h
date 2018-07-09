#ifndef JICDITEM_H
#define JICDITEM_H

#include "jicdobject.h"

namespace Icd {
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
    Q_PROPERTY(qreal localOffset READ localOffset NOTIFY localOffsetChanged)
    Q_PROPERTY(qreal data READ data WRITE setData NOTIFY dataChanged)
    Q_PROPERTY(double defaultValue READ defaultValue WRITE setDefaultValue NOTIFY defaultValueChanged)
    Q_PROPERTY(QString fullValue READ fullValue NOTIFY fullValueChanged)
    Q_PROPERTY(QString typeString READ typeString NOTIFY typeStringChanged)
    Q_PROPERTY(QString dataString READ dataString NOTIFY dataStringChanged)
    Q_PROPERTY(QString typeName READ typeName NOTIFY typeNameChanged)
public:
    explicit JIcdItem(const Icd::ItemPtr &data, QObject *parent = nullptr);

    static void registerQmlType();
    Icd::ItemPtr metaData() const;

    IcdCore::ItemType itemType() const;
    int itemOffset() const;
    qreal bufferSize() const;
    qreal bufferOffset() const;
    qreal localOffset() const;

    qreal data() const;
    double defaultValue() const;
    virtual QString text() const override;
    virtual QString valueString() const override;
    virtual QString fullValue() const;

    QString typeString() const;
    virtual QString dataString() const;
    virtual QString typeName() const;

    static QString typeString(IcdCore::ItemType type);
    static IcdCore::ItemType stringType(const QString &str);

    static JIcdItem *create(const Icd::ItemPtr &data, QObject *parent = nullptr);

    Q_INVOKABLE virtual void updateData() override;
    Q_INVOKABLE virtual void resetData() override;
    Q_INVOKABLE virtual void clearData() override;

signals:
    void itemTypeChanged(IcdCore::ItemType value);
    void itemOffsetChanged(int value);
    void bufferSizeChanged(qreal value);
    void bufferOffsetChanged(qreal value);
    void localOffsetChanged(qreal value);
    void dataChanged(qreal value);
    void defaultValueChanged(qreal value);
    void fullValueChanged();
    void typeStringChanged();
    void dataStringChanged();
    void typeNameChanged();

public slots:
    void setDefaultValue(qreal value);
    void setData(qreal value);

private:
    Q_DISABLE_COPY(JIcdItem)
    J_DECLARE_PRIVATE(JIcdItem)
};

} // end of namespace icdmeta

#endif // JICDITEM_H
