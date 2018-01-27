#ifndef JICDFRAMECODEITEM_H
#define JICDFRAMECODEITEM_H

#include "jicditem.h"

namespace Icd {
class FrameCodeItem;
typedef JHandlePtr<FrameCodeItem> FrameCodeItemPtr;
}

namespace icdmeta {

class JIcdFrameCodeItemPrivate;
class JIcdFrameItem;

class ICDMETA_EXPORT JIcdFrameCodeItem : public JIcdItem
{
    Q_OBJECT
    Q_PROPERTY(QString dataString READ dataString NOTIFY dataStringChanged)
    Q_PROPERTY(icdmeta::JIcdFrameItem* frame READ metaFrame NOTIFY frameChanged)
public:
    explicit JIcdFrameCodeItem(const Icd::FrameCodeItemPtr &data, QObject *parent = 0);
    ~JIcdFrameCodeItem();

    static void registerQmlType();
    Icd::FrameCodeItemPtr metaData() const;

    QString dataString() const;
    QSharedPointer<icdmeta::JIcdFrameItem> frame() const;
    icdmeta::JIcdFrameItem *metaFrame() const;
    QString text() const;
    QString valueString() const;

    Q_INVOKABLE QString frameCoderTypeString() const;
    static QString frameCodeTypeString(IcdCore::FrameCodeType type);
    static IcdCore::FrameCodeType stringFrameCodeType(const QString &str);

    Q_INVOKABLE void updateData();
    Q_INVOKABLE void resetData();
    Q_INVOKABLE void clearData();

signals:
    void dataStringChanged();
    void frameChanged();

public slots:

private:
    void setFrame(const QSharedPointer<icdmeta::JIcdFrameItem> &frame);

private:
    Q_DISABLE_COPY(JIcdFrameCodeItem)
    J_DECLARE_PRIVATE(JIcdFrameCodeItem)
    friend class JIcdTablePrivate;
};

} // end of namespace icdmeta

#endif // JICDFRAMECODEITEM_H
