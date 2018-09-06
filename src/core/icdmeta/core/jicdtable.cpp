#include "precomp.h"
#include "jicdtable.h"
#include "jicdheaderitem.h"
#include "jicdcheckitem.h"
#include "jicdcounteritem.h"
#include "jicdcomplexitem.h"
#include "jicdframecodeitem.h"
#include "jicdframeitem.h"
#include "icdcore/icd_table.h"
#include "icdcore/icd_item_framecode.h"

namespace icdmeta {

// class JIcdTablePrivate

class JIcdTablePrivate
{
public:
    JIcdTablePrivate(JIcdTable *q)
        : J_QPTR(q)
        , data(nullptr)
    {

    }

    void init();
    static int frameCodeCount(QQmlListProperty<JIcdFrameCodeItem> *property);
    static JIcdFrameCodeItem *frameCodeAt(QQmlListProperty<JIcdFrameCodeItem> *property, int index);
    void saveItem(const QSharedPointer<JIcdItem> &item);
    void removeItem(const QSharedPointer<JIcdItem> &item);
    void clearItem();

private:
    J_DECLARE_PUBLIC(JIcdTable)
    Icd::TablePtr data;
    QVector<QSharedPointer<JIcdItem> > items;
    QVector<char> headers;
    QSharedPointer<JIcdCounterItem> counterItem;
    QSharedPointer<JIcdCheckItem> checkItem;
    QList<QSharedPointer<JIcdFrameCodeItem> > frameCodes;
};

void JIcdTablePrivate::init()
{
    Q_Q(JIcdTable);
    items.clear();
    frameCodes.clear();
    QList<QSharedPointer<JIcdFrameItem> > frames;
    for (auto &item : data->allItem()) {
        auto newItem = QSharedPointer<JIcdItem>(JIcdItem::create(item), jdelete_qobject);
        QQmlEngine::setObjectOwnership(newItem.data(), QQmlEngine::CppOwnership);
        if (newItem) {
            items.append(newItem);
            saveItem(newItem);
            if (item->type() == Icd::ItemFrame) {
                frames.append(qSharedPointerDynamicCast<JIcdFrameItem>(newItem));
            }
        } else {
            Q_ASSERT(false);
        }
    }
    // binding frame to framecode
    if (!frameCodes.isEmpty() && !frames.isEmpty()) {
        for (auto &frame : frames) {
            for (auto &frameCode : frameCodes) {
                const auto &metaFrame = frameCode->metaData()->frame();
                if (!metaFrame || metaFrame != frame->metaData()) {
                    continue;
                }
                frameCode->setFrame(frame);
            }
        }
    }
    q->countChanged();
}

int JIcdTablePrivate::frameCodeCount(QQmlListProperty<JIcdFrameCodeItem> *property)
{
    JIcdTable *q = qobject_cast<JIcdTable *>(property->object);
    return q->J_DPTR->frameCodes.count();
}

JIcdFrameCodeItem *JIcdTablePrivate::frameCodeAt(QQmlListProperty<JIcdFrameCodeItem> *property, int index)
{
    JIcdTable *q = qobject_cast<JIcdTable *>(property->object);
    return q->J_DPTR->frameCodes[index].data();
}

void JIcdTablePrivate::saveItem(const QSharedPointer<JIcdItem> &item)
{
    switch (item->itemType()) {
    case IcdCore::ItemHeader:
        headers.append(char(qSharedPointerDynamicCast<JIcdHeaderItem>(item)->defaultValue()));
        break;
    case IcdCore::ItemCounter:
        counterItem = qSharedPointerDynamicCast<JIcdCounterItem>(item);
        break;
    case IcdCore::ItemCheck:
        checkItem = qSharedPointerDynamicCast<JIcdCheckItem>(item);
        break;
    case IcdCore::ItemFrameCode:
        frameCodes.append(qSharedPointerDynamicCast<JIcdFrameCodeItem>(item));
        break;
    case IcdCore::ItemFrame:
        break;
    default:
        break;
    }
}

void JIcdTablePrivate::removeItem(const QSharedPointer<JIcdItem> &item)
{
    switch (item->itemType()) {
    case IcdCore::ItemHeader:
        break;
    case IcdCore::ItemCounter:
        counterItem = nullptr;
        break;
    case IcdCore::ItemCheck:
        checkItem = nullptr;
        break;
    case IcdCore::ItemFrameCode:
    {
        QSharedPointer<JIcdFrameCodeItem> frameCode =
                qSharedPointerDynamicCast<JIcdFrameCodeItem>(item);
        if (!frameCode) {
            break;
        }
        QMutableListIterator<QSharedPointer<JIcdFrameCodeItem> > iter(frameCodes);
        while (iter.hasNext()) {
            if (iter.next() == frameCode) {
                iter.remove();
            }
        }
        break;
    }
    case IcdCore::ItemFrame:
        break;
    default:
        break;
    }
}

void JIcdTablePrivate::clearItem()
{
    headers.clear();
    counterItem = nullptr;
    checkItem = nullptr;
    frameCodes.clear();
}

// class JIcdTable

JIcdTable::JIcdTable(const Icd::TablePtr &data, QObject *parent)
    : JIcdObject(data, parent)
    , J_DPTR(new JIcdTablePrivate(this))
{
    Q_D(JIcdTable);
    d->data = data;
    d->init();
}

JIcdTable::~JIcdTable()
{
    Q_D(JIcdTable);
    delete d;
}

void JIcdTable::registerQmlType()
{
    IcdMetaRegisterUncreatableType2(JIcdTable);
    JIcdItem::registerQmlType();
}

Icd::TablePtr JIcdTable::metaData() const
{
    Q_D(const JIcdTable);
    return d->data;
}

int JIcdTable::itemOffset() const
{
    Q_D(const JIcdTable);
    return d->data->itemOffset();
}

qreal JIcdTable::bufferSize() const
{
    Q_D(const JIcdTable);
    return d->data->bufferSize();
}

qreal JIcdTable::bufferOffset() const
{
    Q_D(const JIcdTable);
    return d->data->bufferOffset();
}

int JIcdTable::count() const
{
    Q_D(const JIcdTable);
    return d->items.count();
}

int JIcdTable::sequence() const
{
    Q_D(const JIcdTable);
    return d->data->sequence();
}

QString JIcdTable::text() const
{
    return "";
}

char *JIcdTable::buffer() const
{
    Q_D(const JIcdTable);
    return d->data->buffer();
}

void JIcdTable::setBuffer(char *buffer)
{
    Q_D(JIcdTable);
    d->data->setBuffer(buffer);
}

icdmeta::JIcdItem *JIcdTable::itemAt(int index) const
{
    Q_D(const JIcdTable);
    return d->items[index].data();
}

icdmeta::JIcdItem *JIcdTable::itemById(const QString &id) const
{
    Q_D(const JIcdTable);
    for (auto &item : d->items) {
        if (item->id() == id) {
            return item.data();
        }
    }

    return nullptr;
}

icdmeta::JIcdObject *JIcdTable::itemByMark(const QString &mark, bool deep) const
{
    Q_D(const JIcdTable);
    for (const auto &item : d->items) {
        const IcdCore::ItemType itemType = item->itemType();
        if (item->mark() == mark) {
            switch (itemType) {
            case IcdCore::ItemComplex:
            {
                const auto complex = qSharedPointerDynamicCast<JIcdComplexItem>(item);
                if (!complex) {
                    break;
                }
                return complex->table();
            }
            default:
                return item.data();
            }
        } else if (deep) {
            switch (itemType) {
            case IcdCore::ItemComplex:
            {
                const auto complex = qSharedPointerDynamicCast<JIcdComplexItem>(item);
                if (!complex) {
                    break;
                }
                const auto childItem = complex->table()->itemByMark(mark, deep);
                if (childItem) {
                    return childItem;
                }
                break;
            }
            case IcdCore::ItemFrame:
            {
                const auto frame = qSharedPointerDynamicCast<JIcdFrameItem>(item);
                if (!frame) {
                    break;
                }
                const auto childItem = frame->itemByMark(mark, deep);
                if (childItem) {
                    return childItem;
                }
                break;
            }
            default:
                break;
            }
        }
    }

    return Q_NULLPTR;
}

icdmeta::JIcdTable *JIcdTable::tableByMark(const QString &mark, bool deep) const
{
    Q_D(const JIcdTable);
    for (const auto &item : d->items) {
        const IcdCore::ItemType itemType = item->itemType();
        if (item->mark() == mark) {
            switch (itemType) {
            case IcdCore::ItemComplex:
            {
                const auto complex = qSharedPointerDynamicCast<JIcdComplexItem>(item);
                if (!complex) {
                    break;
                }
                return complex->table();
            }
            default:
                break;
            }
        } else if (deep) {
            switch (itemType) {
            case IcdCore::ItemComplex:
            {
                const auto complex = qSharedPointerDynamicCast<JIcdComplexItem>(item);
                if (!complex) {
                    break;
                }
                const auto childTable = complex->table()->tableByMark(mark, deep);
                if (childTable) {
                    return childTable;
                }
                break;
            }
            case IcdCore::ItemFrame:
            {
                const auto frame = qSharedPointerDynamicCast<JIcdFrameItem>(item);
                if (!frame) {
                    break;
                }
                const auto childTable = frame->tableByMark(mark, deep);
                if (childTable) {
                    return childTable;
                }
                break;
            }
            default:
                break;
            }
        }
    }

    return Q_NULLPTR;
}

icdmeta::JIcdObject *JIcdTable::itemByDomain(const QString &domain, int domainType) const
{
    Q_D(const JIcdTable);
    if (domain.isEmpty()) {
        return Q_NULLPTR;
    }

    const QString current = domain.section('/', 0, 0);
    if (current.isEmpty()) {
        return Q_NULLPTR;
    }
    const QString nextDomain = domain.section('/', 1);

    for (const auto &item : d->items) {
        if (item->domainOfType(domainType) != current) {
            continue;
        }
        if (nextDomain.isEmpty()) {
            return item.data();
        } else {
            switch (item->itemType()) {
            case IcdCore::ItemComplex:
            {
                const auto complex = qSharedPointerDynamicCast<JIcdComplexItem>(item);
                if (!complex) {
                    break;
                }
                const auto childItem = complex->itemByDomain(nextDomain, domainType);
                if (childItem) {
                    return childItem;
                }
                break;
            }
            case IcdCore::ItemFrame:
            {
                const auto frame = qSharedPointerDynamicCast<JIcdFrameItem>(item);
                if (!frame) {
                    break;
                }
                const auto childItem = frame->itemByDomain(nextDomain, domainType);
                if (childItem) {
                    return childItem;
                }
                break;
            }
            default:
                break;
            }
        }
        break;
    }

    return Q_NULLPTR;
}

icdmeta::JIcdTable *JIcdTable::tableByDomain(const QString &domain,
                                         int domainType) const
{
    JIcdObject *item = itemByDomain(domain, domainType);
    if (!item || item->objectType() != IcdCore::ObjectTable) {
        return Q_NULLPTR;
    }

    return qobject_cast<JIcdTable *>(item);
}

void JIcdTable::updateData()
{
    Q_D(JIcdTable);
    JIcdObject::updateData();
    for (auto &item : d->items) {
        item->updateData();
    }
}

void JIcdTable::resetData()
{
    Q_D(JIcdTable);
    for (auto &item : d->items) {
        item->resetData();
    }
}

void JIcdTable::clearData()
{
    Q_D(JIcdTable);
    for (auto &item : d->items) {
        item->clearData();
    }
}

QVector<char> JIcdTable::headers() const
{
    Q_D(const JIcdTable);
    return QVector<char>::fromStdVector(d->data->headers());
}

QSharedPointer<JIcdCounterItem> JIcdTable::counterItem()
{
    Q_D(JIcdTable);
    return d->counterItem;
}

const QSharedPointer<JIcdCounterItem> &JIcdTable::counterItem() const
{
    Q_D(const JIcdTable);
    return d->counterItem;
}

bool JIcdTable::isCheckValid() const
{
    Q_D(const JIcdTable);
    return d->data->isCheckValid();
}

QSharedPointer<JIcdCheckItem> JIcdTable::checkItem()
{
    Q_D(JIcdTable);
    return d->checkItem;
}

const QSharedPointer<JIcdCheckItem> &JIcdTable::checkItem() const
{
    Q_D(const JIcdTable);
    return d->checkItem;
}

const QList<QSharedPointer<JIcdFrameCodeItem> > &JIcdTable::frameCodes() const
{
    Q_D(const JIcdTable);
    return d->frameCodes;
}

void JIcdTable::updateSend(bool period)
{
    Q_D(JIcdTable);
    d->data->updateSend(period);
}

void JIcdTable::resetSend()
{
    Q_D(JIcdTable);
    d->data->resetSend();
}

void JIcdTable::bindBuffer(void *buffer)
{
    Q_D(JIcdTable);
    d->data->bindBuffer(buffer);
}

QString JIcdTable::typeName() const
{
    Q_D(const JIcdTable);
    return QString::fromStdString(d->data->typeName());
}

QString JIcdTable::codeName() const
{
    Q_D(const JIcdTable);
    return QString::fromStdString(d->data->codeName());
}

QByteArray JIcdTable::metaHeaders() const
{
    Q_D(const JIcdTable);
    QByteArray data;
    foreach (auto &header, d->headers) {
        data.append(header);
    }
    return data;
}

JIcdCounterItem *JIcdTable::metaCounterItem() const
{
    Q_D(const JIcdTable);
    return d->counterItem.data();
}

JIcdCheckItem *JIcdTable::metaCheckItem() const
{
    Q_D(const JIcdTable);
    return d->checkItem.data();
}

QQmlListProperty<JIcdFrameCodeItem> JIcdTable::metaFrameCodes()
{
    return QQmlListProperty<JIcdFrameCodeItem>(this, nullptr,
                                       &JIcdTablePrivate::frameCodeCount,
                                       &JIcdTablePrivate::frameCodeAt);
}

void JIcdTable::setSequence(int value)
{
    Q_D(JIcdTable);
    if (value != d->data->sequence()) {
        d->data->setSequence(value);
        emit sequenceChanged(value);
    }
}

} // end of namespace icdmeta
