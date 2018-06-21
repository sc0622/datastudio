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

    QString toString() const;
    int8_t *i8() const;
    uint8_t *u8() const;
    int16_t *i16() const;
    uint16_t *u16() const;
    int32_t *i32() const;
    uint32_t *u32() const;
    int64_t *i64() const;
    uint64_t *u64() const;
    float_t *f32() const;
    double_t *f64() const;

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
