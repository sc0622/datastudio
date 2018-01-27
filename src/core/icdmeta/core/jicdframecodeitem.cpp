#include "precomp.h"
#include "jicdframecodeitem.h"
#include "jicdframeitem.h"

namespace icdmeta {

// class JIcdFrameCodeItemPrivate

class JIcdFrameCodeItemPrivate
{
public:
    JIcdFrameCodeItemPrivate(JIcdFrameCodeItem *q)
        : q_ptr(q)
        , data(Q_NULLPTR)
        , frame(Q_NULLPTR)
    {

    }

    void init();

private:
    J_DECLARE_PUBLIC(JIcdFrameCodeItem)
    Icd::FrameCodeItemPtr data;
    QSharedPointer<JIcdFrameItem> frame;
};

void JIcdFrameCodeItemPrivate::init()
{

}

// class JIcdFrameCodeItem

JIcdFrameCodeItem::JIcdFrameCodeItem(const Icd::FrameCodeItemPtr &data, QObject *parent)
    : JIcdItem(data, parent)
    , d_ptr(new JIcdFrameCodeItemPrivate(this))
{
    Q_D(JIcdFrameCodeItem);
    d->data = data;
    d->init();
}

JIcdFrameCodeItem::~JIcdFrameCodeItem()
{
    Q_D(JIcdFrameCodeItem);
    delete d;
}

void JIcdFrameCodeItem::registerQmlType()
{
    //
    jRegisterUncreatableType(JIcdFrameCodeItem);

    //
}

Icd::FrameCodeItemPtr JIcdFrameCodeItem::metaData() const
{
    Q_D(const JIcdFrameCodeItem);
    return d->data;
}

QString JIcdFrameCodeItem::dataString() const
{
    Q_D(const JIcdFrameCodeItem);
    return QString::fromStdString(d->data->dataString());
}

QSharedPointer<icdmeta::JIcdFrameItem> JIcdFrameCodeItem::frame() const
{
    Q_D(const JIcdFrameCodeItem);
    return d->frame;
}

JIcdFrameItem *JIcdFrameCodeItem::metaFrame() const
{
    Q_D(const JIcdFrameCodeItem);
    return d->frame.data();
}

QString JIcdFrameCodeItem::text() const
{
    Q_D(const JIcdFrameCodeItem);
    QString info("<font bold=1 face=Consolas>0x");
    // data
    info.append(QString("%1")
                .arg((ushort)(uchar)(d->data->data()),
                     int(d->data->bufferSize() * 2), 16, QChar('0'))
                .toUpper());
    // value
    info.append(", ");
    info.append(QString::number((ushort)(uchar)d->data->data()));
    info.append("</font>");
    return info;
}

QString JIcdFrameCodeItem::valueString() const
{
    Q_D(const JIcdFrameCodeItem);
    return QString::number((ushort)(uchar)d->data->data());
}

QString JIcdFrameCodeItem::frameCoderTypeString() const
{
    Q_D(const JIcdFrameCodeItem);
    return QString::fromStdString(d->data->frameCodeTypeString());
}

QString JIcdFrameCodeItem::frameCodeTypeString(IcdCore::FrameCodeType type)
{
    return QString::fromStdString(Icd::FrameCodeItem::frameCodeTypeString((Icd::FrameCodeType)type));
}

IcdCore::FrameCodeType JIcdFrameCodeItem::stringFrameCodeType(const QString &str)
{
    return (IcdCore::FrameCodeType)Icd::FrameCodeItem::stringFrameCodeType(str.toStdString());
}

void JIcdFrameCodeItem::updateData()
{
    Q_D(JIcdFrameCodeItem);
    d->frame->updateData();
}

void JIcdFrameCodeItem::resetData()
{
    Q_D(JIcdFrameCodeItem);
    d->data->resetData();
}

void JIcdFrameCodeItem::clearData()
{
    Q_D(JIcdFrameCodeItem);
    d->data->clearData();
}

void JIcdFrameCodeItem::setFrame(const QSharedPointer<icdmeta::JIcdFrameItem> &frame)
{
    Q_D(JIcdFrameCodeItem);
    d->frame = frame;
}

} // end of namespace icdmeta
