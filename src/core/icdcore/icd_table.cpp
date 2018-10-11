#include "precomp.h"
#include "icd_table.h"
#include "icd_item_header.h"
#include "icd_item_counter.h"
#include "icd_item_check.h"
#include "icd_item_complex.h"
#include "icd_item_framecode.h"
#include "icd_item_frame.h"
#include "icd_item_bit.h"
#include <assert.h>
#include <deque>
#include "private/jprecitimer.h"

namespace Icd {

typedef std::vector<ComplexItemPtr> ComplexItemPtrArray;

// class TableData

class TableData
{
    friend class Table;
public:
    TableData()
        : sequence(1)
        , itemOffset(1)
        , bufferSize(0.0)
        , bufferOffset(0.0)
        , buffer(nullptr)
        , isFrameTable(false)
        , period(0)
        , counter(nullptr)
        , check(nullptr)
    {

    }

    void saveItem(const ItemPtr &item);
    void removeItem(const ItemPtr &item);
    void clearItem();
    void updateFrameTable(Object *parent);
    void updatePeriod();

    icd_int64 currentMillisecond() const;

    static double recalcBitBufferOffset(const Icd::BitItemPtr &bitItem,
                                        const ItemPtrArray &items,
                                        ItemPtrArray::const_reverse_iterator citer);

private:
    int sequence;
    int itemOffset;             // 数据项偏移量（在所属表中的偏移量）
    double bufferSize;
    double bufferOffset;        //
    char *buffer;
    bool isFrameTable;          //
    icd_int64 period;
    std::deque<icd_int64> times;
    ItemPtrArray items;         // 数据项列表
    CounterItemPtr counter;
    CheckItemPtr check;
    FrameCodeItemPtrArray frameCodes;
    ComplexItemPtrArray complexes;
    std::vector<char> headers;
    JPreciTimer preciTimer;
};

void TableData::saveItem(const ItemPtr &item)
{
    switch (item->type()) {
    case Icd::ItemHeader:
    {
        Icd::HeaderItemPtr headerItem = JHandlePtrCast<Icd::HeaderItem>(item);
        if (!headerItem) {
            break;
        }
        headers.push_back(char(headerItem->defaultValue()));
        break;
    }
    case Icd::ItemCounter:
    {
        counter = JHandlePtrCast<Icd::CounterItem>(item);
        break;
    }
    case Icd::ItemCheck:
    {
        check = JHandlePtrCast<Icd::CheckItem>(item);
        break;
    }
    case Icd::ItemComplex:
    {
        Icd::ComplexItemPtr complexItem = JHandlePtrCast<Icd::ComplexItem>(item);
        if (!complexItem) {
            break;
        }
        complexes.push_back(complexItem);
        break;
    }
    case Icd::ItemFrameCode:
    {
        Icd::FrameCodeItemPtr frameCodeItem = JHandlePtrCast<Icd::FrameCodeItem>(item);
        if (!frameCodeItem) {
            break;
        }
        frameCodes.push_back(frameCodeItem);
        break;
    }
    case Icd::ItemFrame:
        break;
    default:
        break;
    }
}

void TableData::removeItem(const ItemPtr &item)
{
    switch (item->type()) {
    case Icd::ItemCounter:
    {
        counter = Icd::CounterItemPtr();
        break;
    }
    case Icd::ItemCheck:
    {
        check = Icd::CheckItemPtr();
        break;
    }
    case Icd::ItemComplex:
    {
        const Icd::ComplexItemPtr complexItem = JHandlePtrCast<Icd::ComplexItem>(item);
        if (!complexItem) {
            break;
        }

        ComplexItemPtrArray::const_iterator citer = complexes.cbegin();
        for (; citer != complexes.cend(); ++citer) {
            if (complexItem == *citer) {
                complexes.erase(citer);
                break;
            }
        }
        break;
    }
    case Icd::ItemFrameCode:
    {
        const Icd::FrameCodeItemPtr frameCodeItem = JHandlePtrCast<Icd::FrameCodeItem>(item);
        if (!frameCodeItem) {
            break;
        }

        FrameCodeItemPtrArray::const_iterator citer = frameCodes.cbegin();
        for (; citer != frameCodes.cend(); ++citer) {
            if (frameCodeItem == *citer) {
                frameCodes.erase(citer);
                break;
            }
        }
        break;
    }
    case Icd::ItemFrame:
        break;
    default:
        break;
    }
}

void TableData::clearItem()
{
    counter = Icd::CounterItemPtr();
    check = Icd::CheckItemPtr();
    frameCodes.clear();
    complexes.clear();
    headers.clear();
    times.clear();
}

void TableData::updateFrameTable(Object *parent)
{
    isFrameTable = false;

    if (!parent) {
        isFrameTable = true;
        return;
    }

    const Icd::ObjectType objectType = parent->objectType();
    switch (objectType) {
    case Icd::ObjectSystem:
    case Icd::ObjectItem:
    {
        const Icd::Item* item = dynamic_cast<Icd::Item *>(parent);
        if (item && item->type() == Icd::ItemFrameCode) {
            isFrameTable = true;
        }
        break;
    }
    default:
        break;
    }
}

void TableData::updatePeriod()
{
    // calculate period
    if (isFrameTable) {
        icd_int64 msecs = currentMillisecond();
        times.push_back(msecs);
        if (times.size() > 5) {
            times.pop_front();
        }
        // calculate
        size_t size = times.size();
        if (size > 1) {
            period = ((*times.crbegin()) - (*times.cbegin())) / (times.size() - 1);
        }
    }
}

icd_int64 TableData::currentMillisecond() const
{
    return preciTimer.mscount();
}

double TableData::recalcBitBufferOffset(const Icd::BitItemPtr &bitItem,
                                        const ItemPtrArray &items,
                                        ItemPtrArray::const_reverse_iterator citer)
{
    if (!bitItem || citer == items.crend()) {
        return -1;
    }

    const ItemPtr item = *citer;
    if (!item) {
        return -1;
    }

    double offset = item->bufferOffset() + item->bufferSize();

    const ItemType type = item->type();
    if (type != Icd::ItemBitMap && type != Icd::ItemBitValue) {
        return std::ceil(offset);
    }

    const Icd::BitItemPtr _bitItem = JHandlePtrCast<Icd::BitItem>(item);
    if (!_bitItem) {
        return std::ceil(offset);
    }

    if (_bitItem->bitStart() > bitItem->bitStart()) {
        return std::ceil(offset);
    }

    // update typeSize of previous bit item
    _bitItem->setTypeSize(bitItem->typeSize());

    const int calcSize = bitItem->calcSize();
    const int _calcSize = _bitItem->calcSize();
    if (_calcSize == calcSize) {
        //
        return std::floor(offset - _bitItem->bufferSize());
    }

    ++citer;
    if (citer == items.crend()) {
        return offset;
    }

    offset = recalcBitBufferOffset(_bitItem, items, citer);
    if (offset < 0) {
        return offset;
    }

    _bitItem->setBufferOffset(offset);

    return offset;
}

// class Table

Table::Table(Object *parent)
    : Object(ObjectTable, parent)
    , d(new TableData())
{
    d->updateFrameTable(parent);
}

Table::Table(const std::string &id, Object *parent)
    : Object(id, ObjectTable, parent)
    , d(new TableData())
{
    d->updateFrameTable(parent);
}

Table::Table(const Table &other)
    : Object(other)
    , d(new TableData())
{
    *this = other;
}

Table::~Table()
{
    delete d;
}

int Table::rtti() const
{
    return ObjectTable;
}

int Table::itemOffset() const
{
    return d->itemOffset;
}

void Table::setItemOffset(int offset)
{
    d->itemOffset = offset;

    //
    int _offset = itemOffset();
    for (ItemPtrArray::const_iterator citer = d->items.cbegin();
         citer != d->items.cend(); ++citer) {
        const ItemPtr &item = *citer;
        item->setItemOffset(_offset);
        ++_offset;
    }
}

double Table::bufferSize() const
{
    return d->bufferSize;
}

double Table::bufferOffset() const
{
    return d->bufferOffset;
}

void Table::setBufferOffset(double offset)
{
    const double delta = offset - d->bufferOffset;

    d->bufferOffset = offset;

    for (ItemPtrArray::const_iterator citer = d->items.cbegin();
         citer != d->items.cend(); ++citer) {
        const ItemPtr &item = *citer;
        item->setBufferOffset(item->bufferOffset() + delta);
    }
}

char *Table::buffer() const
{
    return d->buffer;
}

void Table::setBuffer(char *buffer)
{
    d->buffer = buffer;

    ItemPtrArray::const_iterator citer = d->items.cbegin();
    if (buffer) {
        char *_buffer = buffer - int(d->bufferOffset);
        for (; citer != d->items.cend(); ++citer) {
            const ItemPtr &item = *citer;
            item->setBuffer(_buffer + int(item->bufferOffset()));
        }
    } else {
        for (; citer != d->items.cend(); ++citer) {
            const ItemPtr &item = *citer;
            item->setBuffer(nullptr);
        }
        d->period = 0;
    }
}

void Table::adjustBufferOffset()
{
    //TODO
}

ItemPtrArray Table::allItem()
{
    return d->items;
}

const ItemPtrArray &Table::allItem() const
{
    return d->items;
}

void Table::appendItem(const ItemPtr &item)
{
    double bufferSize = 0.0;
    if (d->items.empty()) {
        item->setItemOffset(d->itemOffset);
        item->setBufferOffset(d->bufferOffset);
        d->items.push_back(item);
        bufferSize = item->bufferSize();
    } else {
        const ItemPtr &last = *d->items.crbegin();
        item->setItemOffset(last->itemOffset() + 1);

        double offset = 0.0;
        switch (item->type()) {
        case Icd::ItemBitMap:
        case Icd::ItemBitValue:
        {
            const Icd::BitItemPtr bitItem = JHandlePtrCast<Icd::BitItem>(item);
            if (!bitItem) {
                //assert(false);
                return;
            }

            if (bitItem->bitStart() == 0) {
                offset = std::ceil(last->bufferOffset() + last->bufferSize());
            } else {
                offset = TableData::recalcBitBufferOffset(bitItem, d->items, d->items.crbegin());
                if (offset < 0) {
                    offset = std::ceil(last->bufferOffset() + last->bufferSize());
                }
            }

            bufferSize = item->bufferSize();
            break;
        }
        default:
        {
            const ItemType lastType = last->type();
            if (lastType == Icd::ItemBitMap || lastType == Icd::ItemBitValue) {
                const Icd::BitItemPtr bitItem = JHandlePtrCast<Icd::BitItem>(last);
                if (!bitItem) {
                    //assert(false);
                    return;
                }
                offset = std::ceil(last->bufferOffset() + bitItem->typeSize());
            } else {
                offset = std::ceil(last->bufferOffset() + last->bufferSize());
            }
            bufferSize = std::ceil(item->bufferSize());
            break;
        }}
        item->setBufferOffset(offset);
        d->items.push_back(item);
    }

    d->bufferSize += bufferSize;

    d->saveItem(item);
}

void Table::insertItem(int index, const ItemPtr &item)
{
    index = jBound(0, index, int(d->items.size()));
    d->items.insert(d->items.cbegin() + index, item);
    //TODO update offset
}

void Table::removeItem(int index)
{
    Q_UNUSED(index);
    //TODO
}

void Table::removeItemByMark(const std::string &mark)
{
    Q_UNUSED(mark);
    //TODO
}

void Table::clearItem()
{
    d->items.clear();
    d->bufferSize = 0;
    d->clearItem();
}

bool Table::isEmpty() const
{
    return d->items.empty();
}

int Table::itemCount() const
{
    return int(d->items.size());
}

ItemPtr Table::itemAt(int index) const
{
    if (index < 0 || index >= int(d->items.size())) {
        return ItemPtr();
    }

    return d->items.at(size_t(index));
}

ItemPtr Table::itemById(const std::string &id, int rtti) const
{
    for (ItemPtrArray::const_iterator citer = d->items.cbegin();
         citer != d->items.cend(); ++citer) {
        const ItemPtr &item = *citer;
        if ((rtti == ObjectInvalid || item->rtti() == rtti) && item->id() == id) {
            return item;
        }
    }

    return ItemPtr();
}

ObjectPtr Table::itemByName(const std::string &name, bool deep) const
{
    for (ItemPtrArray::const_iterator citer = d->items.cbegin();
         citer != d->items.cend(); ++citer) {
        const ItemPtr &item = *citer;
        const ItemType itemType = item->type();
        if (item->name() == name) {
            switch (itemType) {
            case ItemComplex:
            {
                const ComplexItemPtr complex = JHandlePtrCast<ComplexItem>(item);
                if (!complex) {
                    break;
                }
                return complex->table();
            }
            default:
                return item;
            }
        } else if (deep) {
            switch (itemType) {
            case ItemComplex:
            {
                const ComplexItemPtr complex = JHandlePtrCast<ComplexItem>(item);
                if (!complex) {
                    break;
                }
                const ObjectPtr childItem = complex->table()->itemByName(name, deep);
                if (childItem) {
                    return childItem;
                }
                break;
            }
            case ItemFrame:
            {
                const FrameItemPtr frame = JHandlePtrCast<FrameItem>(item);
                if (!frame) {
                    break;
                }
                const ObjectPtr childItem = frame->itemByName(name, deep);
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

    return ObjectPtr();
}

ObjectPtr Table::itemByMark(const std::string &mark, bool deep) const
{
    for (ItemPtrArray::const_iterator citer = d->items.cbegin();
         citer != d->items.cend(); ++citer) {
        const ItemPtr &item = *citer;
        const ItemType itemType = item->type();
        if(item->mark() == mark) {
            switch (itemType) {
            case ItemComplex:
            {
                const ComplexItemPtr complex = JHandlePtrCast<ComplexItem>(item);
                if (!complex) {
                    break;
                }
                return complex->table();
            }
            default:
                return item;
            }
        } else if (deep) {
            switch (itemType) {
            case ItemComplex:
            {
                const ComplexItemPtr complex = JHandlePtrCast<ComplexItem>(item);
                if (!complex) {
                    break;
                }
                const ObjectPtr childItem = complex->table()->itemByMark(mark, deep);
                if (childItem) {
                    return childItem;
                }
                break;
            }
            case ItemFrame:
            {
                const FrameItemPtr frame = JHandlePtrCast<FrameItem>(item);
                if (!frame) {
                    break;
                }
                const ObjectPtr childItem = frame->itemByMark(mark, deep);
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

    return ObjectPtr();
}

TablePtr Table::tableByMark(const std::string &mark, bool deep) const
{
    for (ItemPtrArray::const_iterator citer = d->items.cbegin();
         citer != d->items.cend(); ++citer) {
        const ItemPtr &item = *citer;
        const ItemType itemType = item->type();
        if(item->mark() == mark) {
            switch (itemType) {
            case ItemComplex:
            {
                const ComplexItemPtr complex = JHandlePtrCast<ComplexItem>(item);
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
            case ItemComplex:
            {
                const ComplexItemPtr complex = JHandlePtrCast<ComplexItem>(item);
                if (!complex) {
                    break;
                }
                const TablePtr childTable = complex->table()->tableByMark(mark, deep);
                if (childTable) {
                    return childTable;
                }
                break;
            }
            case ItemFrame:
            {
                const FrameItemPtr frame = JHandlePtrCast<FrameItem>(item);
                if (!frame) {
                    break;
                }
                const TablePtr childTable = frame->tableByMark(mark, deep);
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

    return TablePtr();
}

ObjectPtr Table::itemByDomain(const std::string &domain, DomainType domainType,
                              bool ignoreComplex) const
{
    if (domain.empty()) {
        return ObjectPtr();
    }

    std::string::size_type index = domain.find_first_of('/');
    std::string current, next;
    if (index == std::string::npos) {
        current = domain;
    } else {
        current = domain.substr(0, index);
        next = domain.substr(index + 1);
    }

    for (ItemPtrArray::const_iterator citer = d->items.cbegin();
         citer != d->items.cend(); ++citer) {
        const ItemPtr &item = *citer;
        const ItemType itemType = item->type();
        if(item->domainOfType(domainType) == current) {
            if (next.empty()) {
                switch (itemType) {
                case ItemComplex:
                {
                    const ComplexItemPtr complex = JHandlePtrCast<ComplexItem>(item);
                    if (!complex) {
                        break;
                    }
                    if (ignoreComplex) {
                        return complex->table();
                    } else {
                        return complex;
                    }
                }
                default:
                    return item;
                }
            } else {
                switch (itemType) {
                case ItemComplex:
                {
                    const ComplexItemPtr complexItem = JHandlePtrCast<ComplexItem>(item);
                    if (!complexItem) {
                        break;
                    }

                    const ObjectPtr childItem = complexItem->table()->itemByDomain(next, domainType, ignoreComplex);
                    if (childItem) {
                        return childItem;
                    }
                    break;
                }
                case ItemFrame:
                {
                    const FrameItemPtr frameItem = JHandlePtrCast<FrameItem>(item);
                    if (!frameItem) {
                        break;
                    }

                    const ObjectPtr childItem = frameItem->itemByDomain(next, domainType, ignoreComplex);
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
    }

    return ObjectPtr();
}

TablePtr Table::tableByDomain(const std::string &domain, Icd::DomainType domainType) const
{
    if (domain.empty()) {
        return TablePtr();
    }

    std::string::size_type index = domain.find_first_of('/');
    std::string current, next;
    if (index == std::string::npos) {
        current = domain;
    } else {
        current = domain.substr(0, index);
        next = domain.substr(index + 1);
    }

    for (ItemPtrArray::const_iterator citer = d->items.cbegin();
         citer != d->items.cend(); ++citer) {
        const ItemPtr &item = *citer;
        const ItemType itemType = item->type();
        if (item->domainOfType(domainType) == current) {
            if (next.empty()) {
                switch (itemType) {
                case ItemComplex:
                {
                    const ComplexItemPtr complex = JHandlePtrCast<ComplexItem>(item);
                    if (!complex) {
                        break;
                    }
                    return complex->table();
                }
                default:
                    return TablePtr();
                }
            } else {
                switch (itemType) {
                case ItemComplex:
                {
                    const ComplexItemPtr complexItem = JHandlePtrCast<ComplexItem>(item);
                    if (!complexItem) {
                        break;
                    }

                    const TablePtr childTable = complexItem->table()->tableByDomain(next, domainType);
                    if (childTable) {
                        return childTable;
                    }
                    break;
                }
                case ItemFrame:
                {
                    const FrameItemPtr frameItem = JHandlePtrCast<FrameItem>(item);
                    if (!frameItem) {
                        break;
                    }

                    const TablePtr childTable = frameItem->tableByDomain(next, domainType);
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
    }

    return TablePtr();
}

ObjectPtr Table::findByDomain(const std::string &domain, int domainType, bool ignoreComplex) const
{
    return itemByDomain(domain, Icd::DomainType(domainType), ignoreComplex);
}

bool Table::hasChildByName(const std::string &name, const Icd::ObjectPtr &exclude) const
{
    for (ItemPtrArray::const_iterator citer = d->items.cbegin();
         citer != d->items.cend(); ++citer) {
        const ItemPtr &item = *citer;
        if (exclude && item->id() == exclude->id()) {
            continue;
        }
        if (item->name() == name) {
            return true;
        }
    }

    return false;
}

bool Table::hasChildByMark(const std::string &mark, const Icd::ObjectPtr &exclude) const
{
    for (ItemPtrArray::const_iterator citer = d->items.cbegin();
         citer != d->items.cend(); ++citer) {
        const ItemPtr &item = *citer;
        if (exclude && item->id() == exclude->id()) {
            continue;
        }
        if (item->mark() == mark) {
            return true;
        }
    }

    return false;
}

ObjectPtr Table::childAt(icd_uint64 index) const
{
    return itemAt(int(index));
}

ObjectPtr Table::replaceChild(icd_uint64 index, ObjectPtr &other)
{
    if (index >= d->items.size()) {
        return ObjectPtr();
    }

    if (!other || other->objectType() != ObjectItem) {
        return ObjectPtr();
    }

    ItemPtrArray::iterator iter = d->items.begin() + int(index);
    ItemPtr old = *iter;
    *iter = JHandlePtrCast<Item>(other);

    //TODO update offset ...

    return old;
}

void Table::moveChild(int sourceIndex, int targetIndex)
{
    if (sourceIndex == targetIndex) {
        return;
    }

    if (sourceIndex < 0 || sourceIndex >= int(d->items.size())) {
        return;
    }

    if (targetIndex < 0 || targetIndex >= int(d->items.size())) {
        return;
    }

    ItemPtrArray::const_iterator citer = d->items.cbegin() + sourceIndex;
    ItemPtr item = *citer;
    d->items.erase(citer);
    d->items.insert(d->items.begin() + targetIndex, item);

    // update offset
    adjustBufferOffset();
}

void Table::removeChild(icd_uint64 index)
{
    if (index >= d->items.size()) {
        return;
    }

    d->items.erase(d->items.cbegin() + int(index));
}

void Table::clearChildren()
{
    d->items.clear();
}

const std::vector<char> &Table::headers() const
{
    return d->headers;
}

CounterItemPtr Table::counterItem()
{
    return d->counter;
}

bool Table::isCheckValid() const
{
    return (d->check)
            && (d->check->isValid())
            && (d->check->endPos() < (this->bufferSize() - 1));
}

CheckItemPtr Table::checkItem()
{
    return d->check;
}

const CheckItemPtr &Table::checkItem() const
{
    return d->check;
}

const Icd::FrameCodeItemPtrArray &Icd::Table::allFrameCode() const
{
    return d->frameCodes;
}

void Table::updateSend(bool period)
{
    if (period) {
        d->updatePeriod();
    }

    Icd::FrameCodeItemPtrArray::const_iterator citerFrameCode = d->frameCodes.cbegin();
    for (; citerFrameCode != d->frameCodes.cend(); ++citerFrameCode) {
        const Icd::FrameCodeItemPtr &frameCode = *citerFrameCode;
        if (frameCode) {
            frameCode->updateSend(period);
        }
    }

    Icd::ComplexItemPtrArray::const_iterator citerComplex = d->complexes.cbegin();
    for (; citerComplex != d->complexes.cend(); ++citerComplex) {
        const Icd::ComplexItemPtr &complex = *citerComplex;
        if (complex) {
            complex->updateSend(period);
        }
    }
}

void Table::updateRecv()
{
    d->updatePeriod();

    Icd::FrameCodeItemPtrArray::const_iterator citer = d->frameCodes.cbegin();
    for (; citer != d->frameCodes.cend(); ++citer) {
        const Icd::FrameCodeItemPtr &frameCode = *citer;
        if (frameCode) {
            frameCode->updateRecv();
        }
    }

    Icd::ComplexItemPtrArray::const_iterator citerComplex = d->complexes.cbegin();
    for (; citerComplex != d->complexes.cend(); ++citerComplex) {
        const Icd::ComplexItemPtr &complex = *citerComplex;
        if (complex) {
            complex->updateRecv();
        }
    }
}

void Table::resetSend()
{
    Object::resetData();

    d->times.clear();
    d->period = 0;

    Icd::FrameCodeItemPtrArray::const_iterator citer = d->frameCodes.cbegin();
    for (; citer != d->frameCodes.cend(); ++citer) {
        const Icd::FrameCodeItemPtr &frameCode = *citer;
        if (frameCode) {
            frameCode->resetSend();
        }
    }

    Icd::ComplexItemPtrArray::const_iterator citerComplex = d->complexes.cbegin();
    for (; citerComplex != d->complexes.cend(); ++citerComplex) {
        const Icd::ComplexItemPtr &complex = *citerComplex;
        if (complex) {
            complex->resetSend();
        }
    }
}

void Table::bindBuffer(void *buffer)
{
    (void)(buffer);
}

int Table::sequence() const
{
    return d->sequence;
}

void Table::setSequence(int sequence)
{
    d->sequence = sequence < 0 ? 1 : sequence;
}

int Table::period() const
{
    return static_cast<int>(d->period);
}

bool Table::isFrameTable() const
{
    return d->isFrameTable;
}

bool Table::isSubFrameTable() const
{
    Object *parent = this->parent();
    if (parent && parent->rtti() == Icd::ObjectFrame) {
        return true;
    } else {
        return false;
    }
}

int Table::frameCodeType() const
{
    if (!isSubFrameTable()) {
        return Icd::FrameCodeInvalid;
    }

    switch (mark().size()) {
    case 2: return Icd::FrameCodeU8;
    case 4: return Icd::FrameCodeU16;
    case 8: return Icd::FrameCodeU32;
    case 16: return Icd::FrameCodeU64;
    default: return Icd::FrameCodeInvalid;
    }
}

icd_uint64 Table::frameCode() const
{
    return Icd::strtou64(mark().c_str(), 16);
}

std::string Table::typeName() const
{
    std::string prefix;
    Icd::Item *parentItem = parent() ? dynamic_cast<Icd::Item *>(parent()) : nullptr;
    if (parentItem) {
        if (parentItem->type() == Icd::ItemComplex) {
            prefix = "Complex_";
        } else {
            prefix = "Table_";
        }
    } else {
        prefix = "Table_";
    }

    return prefix + mark();
}

std::string Table::codeName() const
{
    Icd::Item *parentItem = parent() ? dynamic_cast<Icd::Item *>(parent()) : nullptr;
    if (parentItem) {
        if (parentItem->type() == Icd::ItemComplex) {
            return mark();
        } else {
            return parentItem->codeName() + "_" + mark();
        }
    } else {
        return mark();
    }
}

void Table::setParent(Object *parent)
{
    Object::setParent(parent);
    d->updateFrameTable(parent);
}

int Table::childCount() const
{
    int count = 0;

    for (ItemPtrArray::const_iterator citer = d->items.cbegin();
         citer != d->items.end(); ++citer) {
        const ItemPtr &item = *citer;
        switch (item->type()) {
        case Icd::ItemComplex:
        case Icd::ItemFrame:
            count += item->childCount();
            break;
        default:
            ++count;
            break;
        }
    }

    return count;
}

void Table::resetData()
{
    for (ItemPtrArray::const_iterator citer = d->items.cbegin();
         citer != d->items.end(); ++citer) {
        (*citer)->resetData();
    }
}

void Table::clearData()
{
    if (d->buffer) {
        memset(d->buffer, 0, size_t(std::ceil(bufferSize())));
    }

    for (FrameCodeItemPtrArray::const_iterator citer = d->frameCodes.cbegin();
         citer != d->frameCodes.end(); ++citer) {
        (*citer)->clearData();
    }

    for (ComplexItemPtrArray::const_iterator citer = d->complexes.cbegin();
         citer != d->complexes.end(); ++citer) {
        (*citer)->clearData();
    }
}

ObjectPtr Table::copy() const
{
    return std::make_shared<Table>(*this);
}

ObjectPtr Table::clone() const
{
    TablePtr newTable = std::make_shared<Table>(*this);
    // children
    const ItemPtrArray &items = d->items;
    for (ItemPtrArray::const_iterator citer = items.cbegin();
         citer != items.cend(); ++citer) {
        const ItemPtr &item = *citer;
        if (item->type() == Icd::ItemFrame) {
            continue;
        }
        ItemPtr newItem = JHandlePtrCast<Item>((*citer)->clone());
        newItem->setParent(newTable.get());
        newTable->appendItem(newItem);
    }
    return newTable;
}

Table &Table::operator =(const Table &other)
{
    if (this == &other) {
        return *this;
    }
    Object::operator =(other);
    d->sequence = other.d->sequence;
    d->itemOffset = other.d->itemOffset;
    d->bufferSize = other.d->bufferSize;
    d->bufferOffset = other.d->bufferOffset;
    d->buffer = other.d->buffer;
    d->isFrameTable = other.d->isFrameTable;
    d->items = other.allItem();
    d->counter = other.d->counter;
    d->check = other.d->check;
    d->frameCodes = other.d->frameCodes;
    d->complexes = other.d->complexes;
    d->headers = other.d->headers;
    return *this;
}

double Table::recalcBitBufferOffset(const BitItemPtr &bitItem,
                                    const ItemPtrArray &items,
                                    ItemPtrArray::const_reverse_iterator citer)
{
    return TableData::recalcBitBufferOffset(bitItem, items, citer);
}

Json::Value Table::save() const
{
    Json::Value json = Object::save();

    json["sequence"] = sequence();

    if (!d->items.empty()) {
        Json::Value itemsJson;
        for (ItemPtrArray::const_iterator citer = d->items.cbegin();
             citer != d->items.cend(); ++citer) {
            const ItemPtr &item = *citer;
            if (item) {
                itemsJson.append(item->save());
            }
        }
        json["items"] = itemsJson;
    }

    return json;
}

bool Table::restore(const Json::Value &json, int deep)
{
    if (!Object::restore(json, deep)) {
        return false;
    }

    clearItem();

    setSequence(json["sequence"].asInt());
    setItemOffset(1);
    setBufferOffset(0);
    setBuffer(nullptr);
    d->period = 0;

    if (deep <= Icd::ObjectTable) {
        return true;
    }

    if (json.isMember("items")) {
        Json::Value itemsJson = json["items"];
        for (Json::ValueConstIterator citer = itemsJson.begin();
             citer != itemsJson.end(); ++citer) {
            const Json::Value &itemJson = *citer;
            Icd::ItemPtr item = Item::create(Item::stringType(itemJson["type"].asString()), this);
            if (!item) {
                continue;
            }
            if (!item->restore(itemJson, deep)) {
                continue;
            }
            appendItem(item);
        }
        // link frame and frame code
        Icd::FrameCodeItemPtrArray::const_iterator citer = d->frameCodes.cbegin();
        for (; citer != d->frameCodes.cend(); ++citer) {
            const Icd::FrameCodeItemPtr &frameCode = *citer;
            const std::string frameId = frameCode->frameId();
            if (frameId.empty()) {
                continue;
            }
            // find frame
            Icd::FrameItemPtr frame = JHandlePtrCast<Icd::FrameItem>(itemById(frameId, ObjectFrame));
            if (!frame) {
                continue;
            }
            // frame attribute
            frame->setParent(frameCode.get());
            // binding
            frameCode->setFrame(frame);
        }
    }

    return true;
}

} // end of namespace Icd
