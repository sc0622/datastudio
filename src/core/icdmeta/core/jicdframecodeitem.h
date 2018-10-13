#ifndef JICDFRAMECODEITEM_H
#define JICDFRAMECODEITEM_H

#include "jicditem.h"

namespace Icd {
class FrameCodeItem;
typedef std::shared_ptr<FrameCodeItem> FrameCodeItemPtr;
}

namespace icdmeta {

class JIcdFrameCodeItemPrivate;
class JIcdFrameItem;

class ICDMETA_EXPORT JIcdFrameCodeItem : public JIcdItem
{
    Q_OBJECT
    Q_PROPERTY(IcdCore::FrameCodeType frameCodeType READ frameCodeType NOTIFY frameCodeTypeChanged)
    Q_PROPERTY(icdmeta::JIcdFrameItem* frame READ metaFrame NOTIFY frameChanged)
public:
    explicit JIcdFrameCodeItem(const Icd::FrameCodeItemPtr &data, QObject *parent = nullptr);
    ~JIcdFrameCodeItem() override;

    static void registerQmlType();
    Icd::FrameCodeItemPtr metaData() const;

    IcdCore::FrameCodeType frameCodeType() const;
    Q_INVOKABLE QString frameCoderTypeString() const;
    static QString frameCodeTypeString(IcdCore::FrameCodeType type);
    static IcdCore::FrameCodeType stringFrameCodeType(const QString &str);

    QSharedPointer<icdmeta::JIcdFrameItem> frame() const;
    icdmeta::JIcdFrameItem *metaFrame() const;

    QString text() const override;
    QString valueString() const override;
    QString dataString() const override;

    void updateData() override;
    void resetData() override;
    void clearData() override;

signals:
    void frameCodeTypeChanged();
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
