#include "precomp.h"
#include "icd_table.h"
#include "icd_item_header.h"
#include "icd_item_counter.h"
#include "icd_item_check.h"
#include "icd_item_complex.h"
#include "icd_item_framecode.h"
#include "icd_item_frame.h"
#include "icd_item_bit.h"
#include "icd_item_array.h"
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
        , bufferSize(0)
        , bufferOffset(0)
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

    static int recalcOffset(const BitItemPtr &currentBit, const ItemPtrArray &items,
                            ItemPtrArray::const_iterator previousCiter);

    static bool adjustOffset(const ItemPtrArray &items, int itemOffset, int bufferOffset,
                             const ItemPtr &currentItem, ItemPtrArray::const_iterator currentCiter);

private:
    int sequence;
    int itemOffset;             // 数据项偏移量（在所属表中的偏移量）
    int bufferSize;
    int bufferOffset;           //
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

int TableData::recalcOffset(const BitItemPtr &currentBit, const ItemPtrArray &items,
                            ItemPtrArray::const_iterator previousCiter)
{
    if (!currentBit || previousCiter == items.cend()) {
        return -1;
    }

    const ItemPtr &previousItem = *previousCiter;
    if (!previousItem) {
        return -1;
    }

    const ItemType itemType = previousItem->type();
    if (itemType != ItemBitMap && itemType != ItemBitValue) {
        return -1;
    }

    const BitItemPtr previousBit = JHandlePtrCast<BitItem>(previousItem);
    if (!previousBit) {
        return -1;
    }

    if (previousBit->bitStart() + previousBit->bitCount() > currentBit->bitStart()) {
        return -1;  // start from 0 again
    }

    previousBit->setBufferSize(currentBit->bufferSize());

    if (previousCiter == items.cbegin()) {
        return previousBit->bufferOffset();
    }

    --previousCiter;

    int bufferOffset = recalcOffset(previousBit, items, previousCiter);
    if (bufferOffset < 0) {
        return previousBit->bufferOffset();
    }

    previousBit->setBufferOffset(bufferOffset);

    return bufferOffset;
}

bool TableData::adjustOffset(const ItemPtrArray &items, int itemOffset, int bufferOffset,
                             const ItemPtr &currentItem, ItemPtrArray::const_iterator currentCiter)
{
    if (!currentItem) {
        return false;
    }

    if (items.empty() || currentCiter == items.cbegin()) {
        currentItem->setItemOffset(itemOffset);
        currentItem->setBufferOffset(bufferOffset);
    } else {
        ItemPtrArray::const_iterator previousCiter = currentCiter - 1;
        const ItemPtr &previousItem = *previousCiter;
        currentItem->setItemOffset(previousItem->itemOffset() + 1);
        int bufferOffset = previousItem->bufferOffset() + previousItem->bufferSize();

        switch (currentItem->type()) {
        case Icd::ItemBitMap:
        case Icd::ItemBitValue:
        {
            const Icd::BitItemPtr currentBit = JHandlePtrCast<Icd::BitItem>(currentItem);
            if (!currentBit) {
                return false;
            }

            if (currentBit->bitStart() > 0) {
                const int offset = recalcOffset(currentBit, items, previousCiter);
                if (offset >= 0) {
                    bufferOffset = offset;
                }
            }
            break;
        }
        default:
            break;
        }

        currentItem->setBufferOffset(bufferOffset);
    }

    return true;
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

int Table::bufferSize() const
{
    return d->bufferSize;
}

int Table::bufferOffset() const
{
    return d->bufferOffset;
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
        char *_buffer = buffer - d->bufferOffset;
        for (; citer != d->items.cend(); ++citer) {
            const ItemPtr &item = *citer;
            item->setBuffer(_buffer + item->bufferOffset());
        }
    } else {
        for (; citer != d->items.cend(); ++citer) {
            const ItemPtr &item = *citer;
            item->setBuffer(nullptr);
        }
        d->period = 0;
    }
}

void Table::adjustOffset()
{
    for (ItemPtrArray::const_iterator citer = d->items.cbegin();
         citer != d->items.cend(); ++citer) {
        const ItemPtr &item = *citer;
        TableData::adjustOffset(d->items, d->itemOffset, d->bufferOffset, item, citer);
    }

    if (d->items.empty()) {
        d->bufferSize = 0;
    } else {
        const ItemPtr &last = *d->items.crbegin();
        d->bufferSize = last->bufferOffset() + last->bufferSize();
    }
}

int Table::adjustOffset(const ItemPtrArray &items)
{
    if (items.empty()) {
        return 0;
    }

    for (ItemPtrArray::const_iterator citer = items.cbegin();
         citer != items.cend(); ++citer) {
        const ItemPtr &item = *citer;
        TableData::adjustOffset(items, 0, 0, item, citer);
    }

    const ItemPtr &last = *items.crbegin();
    return last->bufferOffset() + last->bufferSize();
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
    TableData::adjustOffset(d->items, d->itemOffset, d->bufferOffset, item, d->items.cend());
    d->items.push_back(item);
    d->saveItem(item);
    // update buffer size
    d->bufferSize = item->bufferOffset() + item->bufferSize();
}

void Table::insertItem(int index, const ItemPtr &item)
{
    index = jBound(0, index, int(d->items.size()));
    d->items.insert(d->items.cbegin() + index, item);
    // update offset
    adjustOffset();
}

void Table::removeItem(int index)
{
    if (index < 0 || index >= int(d->items.size())) {
        return;
    }

    d->items.erase(d->items.cbegin() + index);

    // update offset
    adjustOffset();
}

void Table::removeItemByMark(const std::string &mark)
{
    for (ItemPtrArray::const_iterator citer = d->items.cbegin();
         citer != d->items.cend(); ++citer) {
        const ItemPtr &item = *citer;
        if (item->mark() == mark) {
            d->items.erase(citer);
            // update offset
            adjustOffset();
            return;
        }
    }
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

    // update offset
    adjustOffset();

    return old;
}

ObjectPtr Table::replaceChild(const std::string &id, ObjectPtr &other)
{
    if (!other || other->objectType() != Icd::ObjectItem) {
        return ObjectPtr();
    }

    const Icd::ItemPtr otherItem = JHandlePtrCast<Icd::Item>(other);
    if (!otherItem) {
        return ObjectPtr();
    }

    for (ItemPtrArray::iterator iter = d->items.begin();
         iter != d->items.end(); ++iter) {
        const ItemPtr &item = *iter;
        if (item->id() != id) {
            continue;
        }
        *iter = otherItem;
        return item;
    }

    return ObjectPtr();
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
    adjustOffset();
}

void Table::removeChild(icd_uint64 beginIndex, int endIndex)
{
    if (beginIndex >= d->items.size()) {
        return;
    }

    if (endIndex >= 0 && int(beginIndex) < endIndex && endIndex < int(d->items.size())) {
        d->items.erase(d->items.cbegin() + int(beginIndex),
                       d->items.cbegin() + endIndex);
    } else {
        d->items.erase(d->items.cbegin() + int(beginIndex));
    }

    // update offset
    adjustOffset();
}

void Table::removeChild(const std::list<icd_uint64> &indexes)
{
    std::list<icd_uint64> sortedIndexes = indexes;
    sortedIndexes.sort(std::greater<icd_uint64>());

    bool modified = false;
    for (std::list<icd_uint64>::const_iterator citer = sortedIndexes.cbegin();
         citer != sortedIndexes.cend(); ++citer) {
        const icd_uint64 index = *citer;
        if (index < d->items.size()) {
            d->items.erase(d->items.cbegin() + int(index));
            modified = true;
        }
    }

    // update offset
    if (modified) {
        adjustOffset();
    }
}

void Table::clearChildren()
{
    clearItem();
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
            && (d->check->endPos() < (d->bufferSize - 1));
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
    return int(d->period);
}

bool Table::isFrameTable() const
{
    return d->isFrameTable;
}

bool Table::isSubFrameTable() const
{
    Object *_parent = parent();
    if (_parent && _parent->rtti() == Icd::ObjectFrame) {
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
        ItemType itemType = item->type();
        if (itemType == Icd::ItemComplex || itemType == Icd::ItemFrame) {
            count += item->childCount();
        } else {
            ++count;
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
        memset(d->buffer, 0, size_t(bufferSize()));
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
    TablePtr newTable = std::make_shared<Table>(*this);
    return newTable;
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
        const Json::Value &itemsJson = json["items"];
        for (Json::ValueConstIterator citer = itemsJson.begin();
             citer != itemsJson.end(); ++citer) {
            const Json::Value &itemJson = *citer;
            Icd::ItemPtr item = Item::create(itemCount() + 1, itemJson, deep, this);
            if (!item) {
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

void Table::setBufferOffset(int offset)
{
    const int delta = offset - d->bufferOffset;

    d->bufferOffset = offset;

    for (ItemPtrArray::const_iterator citer = d->items.cbegin();
         citer != d->items.cend(); ++citer) {
        const ItemPtr &item = *citer;
        item->setBufferOffset(item->bufferOffset() + delta);
    }
}

} // end of namespace Icd
