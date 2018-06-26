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
    QString dataString() const override;

    qint8 *i8() const;
    quint8 *u8() const;
    qint16 *i16() const;
    quint16 *u16() const;
    qint32 *i32() const;
    quint32 *u32() const;
    qint64 *i64() const;
    quint64 *u64() const;
    float *f32() const;
    double *f64() const;

    void setData(const char *buffer, int size);
    void setData(const QByteArray &data);
    void setData(const QString &text);

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
