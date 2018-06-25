#include "precomp.h"
#include "icd_table.h"
#include "icd_item_header.h"
#include "icd_item_counter.h"
#include "icd_item_check.h"
#include "icd_item_complex.h"
#include "icd_item_framecode.h"
#include "icd_item_frame.h"
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
        , buffer(0)
        , isFrameTable(false)
        , period(0)
        , itemCounter(0)
        , itemCheck(0)
    {

    }

    void saveItem(const ItemPtr &item);
    void removeItem(const ItemPtr &item);
    void clearItem();
    void updateFrameTable(Object *parent);
    void updatePeriod();

    icd_int64 currentMillisecond() const;

private:
    int sequence;
    int itemOffset;             // 数据项偏移量（在所属表中的偏移量）
    double bufferSize;
    double bufferOffset;        //
    char *buffer;
    bool isFrameTable;
    icd_int64 period;
    std::deque<icd_int64> times;
    ItemPtrArray items;         // 数据项列表
    CounterItemPtr itemCounter;
    CheckItemPtr itemCheck;
    FrameCodeItemPtrArray frameCodes;
    ComplexItemPtrArray complexes;
    std::vector<char> headers;
    JPreciTimer preciTimer;
};

void TableData::saveItem(const ItemPtr &item)
{
    switch (item->type()) {
    case Icd::ItemHead:
    {
        Icd::HeaderItemPtr headerItem = JHandlePtrCast<Icd::HeaderItem, Icd::Item>(item);
        if (!headerItem) {
            break;
        }
        headers.push_back(char(headerItem->defaultValue()));
        break;
    }
    case Icd::ItemCounter:
    {
        itemCounter = JHandlePtrCast<Icd::CounterItem, Icd::Item>(item);
        break;
    }
    case Icd::ItemCheck:
    {
        itemCheck = JHandlePtrCast<Icd::CheckItem, Icd::Item>(item);
        break;
    }
    case Icd::ItemComplex:
    {
        Icd::ComplexItemPtr complexItem = JHandlePtrCast<Icd::ComplexItem, Icd::Item>(item);
        if (!complexItem) {
            break;
        }
        complexes.push_back(complexItem);
        break;
    }
    case Icd::ItemFrameCode:
    {
        Icd::FrameCodeItemPtr frameCodeItem = JHandlePtrCast<Icd::FrameCodeItem, Icd::Item>(item);
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
        itemCounter = Icd::CounterItemPtr(0);
        break;
    }
    case Icd::ItemCheck:
    {
        itemCheck = Icd::CheckItemPtr(0);
        break;
    }
    case Icd::ItemComplex:
    {
        const Icd::ComplexItemPtr complexItem = JHandlePtrCast<Icd::ComplexItem, Icd::Item>(item);
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
        const Icd::FrameCodeItemPtr frameCodeItem = JHandlePtrCast<Icd::FrameCodeItem, Icd::Item>(item);
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
    itemCounter = Icd::CounterItemPtr(0);
    itemCheck = Icd::CheckItemPtr(0);
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
    d->bufferOffset = offset;

    //
    double _offset = d->bufferOffset;
    for (ItemPtrArray::const_iterator citer = d->items.cbegin();
         citer != d->items.cend(); ++citer) {
        const ItemPtr &item = *citer;
        switch (item->type()) {
        case Icd::ItemBitMap:
        case Icd::ItemBitValue:
        {
            item->setBufferOffset(_offset);
            break;
        }
        default:
            _offset = std::ceil(_offset);
            item->setBufferOffset(_offset);
            break;
        }
        _offset += item->bufferSize();
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
        char *_buffer = buffer - (int)d->bufferOffset;
        for (; citer != d->items.cend(); ++citer) {
            const ItemPtr &item = *citer;
            item->setBuffer(_buffer + (int)item->bufferOffset());
        }
    } else {
        for (; citer != d->items.cend(); ++citer) {
            const ItemPtr &item = *citer;
            item->setBuffer(0);
        }
        d->period = 0;
    }
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
    item->setParent(this);
    // append
    if (d->items.empty()) {
        item->setItemOffset(d->itemOffset);
        item->setBufferOffset(d->bufferOffset);
        d->items.push_back(item);
    } else {
        const ItemPtr &last = *d->items.crbegin();
        item->setItemOffset(last->itemOffset() + 1);
        const double offset = last->bufferOffset() + last->bufferSize();
        switch (item->type()) {
        case Icd::ItemBitMap:
        case Icd::ItemBitValue:
        {
            item->setBufferOffset(offset);
            break;
        }
        default:
            item->setBufferOffset((int)std::ceil(offset));
            break;
        }
        d->items.push_back(item);
    }

    // update self buffsetSize
    d->bufferSize += item->bufferSize();

    // save special item
    d->saveItem(item);
}

void Table::insertItem(int index, const ItemPtr &item)
{
    if (index < 0 || index >= (int)d->items.size()) {
        assert(false);
        return;
    }

    item->setParent(this);

    // get offset iterator
    ItemPtrArray::const_iterator citer = d->items.cbegin() + index;

    // initialize itemOffset
    int itemOffset = (*citer)->itemOffset();
    // initialize bufferOffset
    int bufferOffset = int((*citer)->bufferOffset());

    // insert item
    d->items.insert(citer, item);

    // update offset
    for (; citer != d->items.cend(); ++citer) {
        const ItemPtr &next = *citer;
        next->setItemOffset(itemOffset);
        next->setBufferOffset(bufferOffset);
        ++itemOffset;
        bufferOffset += int(next->bufferSize());
    }

    // update self buffsetSize
    d->bufferSize += item->bufferSize();

    // save special item
    d->saveItem(item);
}

void Table::removeItem(int index)
{
    if (index < 0 || index >= (int)d->items.size()) {
        return;     // overflow
    }

    // get offset iterator
    ItemPtrArray::const_iterator citer = d->items.cbegin() + index;

    // erase item
    d->items.erase(citer);

    // update offset

    // update self buffsetSize
    d->bufferSize -= (*citer)->bufferSize();

    // remove special item
    d->removeItem(*citer);
}

void Table::removeItemById(const std::string &id)
{
    for (ItemPtrArray::const_iterator citer = d->items.cbegin();
         citer != d->items.cend(); ++citer) {
        //
        const ItemPtr &item = *citer;
        if(item->id() == id) {
            // erase item
            d->items.erase(citer);
            // update self buffsetSize
            d->bufferSize -= item->bufferSize();
            // remove special item
            d->removeItem(item);
            break;
        }
    }
}

void Table::removeItemByMark(const std::string &mark)
{
    for (ItemPtrArray::const_iterator citer = d->items.cbegin();
         citer != d->items.cend(); ++citer) {
        //
        const ItemPtr &item = *citer;
        if(item->mark() == mark) {
            // erase item
            d->items.erase(citer);
            // update self buffsetSize
            d->bufferSize -= item->bufferSize();
            // remove special item
            d->removeItem(item);
            break;
        }
    }
}

void Table::clearItem()
{
    // clear
    d->items.clear();

    // update bufferSize
    d->bufferSize = 0;

    // clear special items
    d->clearItem();
}

bool Table::isEmpty() const
{
    return d->items.empty();
}

int Table::itemCount() const
{
    return (int)d->items.size();
}

ItemPtr Table::itemAt(int index) const
{
    if (index < 0 || index >= (int)d->items.size()) {
        assert(false);
        return ItemPtr(0);
    }

    return d->items.at(index);
}

ItemPtr Table::itemById(const std::string &id) const
{
    for (ItemPtrArray::const_iterator citer = d->items.cbegin();
         citer != d->items.cend(); ++citer) {
        const ItemPtr &item = *citer;
        if(item->id() == id) {
            return item;
        }
    }

    return ItemPtr(0);
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
                const ComplexItemPtr complex = JHandlePtrCast<ComplexItem, Item>(item);
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
                const ComplexItemPtr complex = JHandlePtrCast<ComplexItem, Item>(item);
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
                const FrameItemPtr frame = JHandlePtrCast<FrameItem, Item>(item);
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

    return ObjectPtr(0);
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
                const ComplexItemPtr complex = JHandlePtrCast<ComplexItem, Item>(item);
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
                const ComplexItemPtr complex = JHandlePtrCast<ComplexItem, Item>(item);
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
                const FrameItemPtr frame = JHandlePtrCast<FrameItem, Item>(item);
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
                    const ComplexItemPtr complex = JHandlePtrCast<ComplexItem, Item>(item);
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
                    const ComplexItemPtr complexItem = JHandlePtrCast<ComplexItem, Item>(item);
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
                    const FrameItemPtr frameItem = JHandlePtrCast<FrameItem, Item>(item);
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
                    const ComplexItemPtr complex = JHandlePtrCast<ComplexItem, Item>(item);
                    if (!complex) {
                        break;
                    }
                    return complex->table();
                }
                default:
                    return TablePtr(0);
                }
            } else {
                switch (itemType) {
                case ItemComplex:
                {
                    const ComplexItemPtr complexItem = JHandlePtrCast<ComplexItem, Item>(item);
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
                    const FrameItemPtr frameItem = JHandlePtrCast<FrameItem, Item>(item);
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

    return TablePtr(0);
}

const std::vector<char> &Table::headers() const
{
    return d->headers;
}

CounterItemPtr Table::counterItem()
{
    return d->itemCounter;
}

bool Table::isCheckValid() const
{
    return (d->itemCheck)
            && (d->itemCheck->isValid())
            && (d->itemCheck->endPos() < (this->bufferSize() - 1));
}

CheckItemPtr Table::checkItem()
{
    return d->itemCheck;
}

const CheckItemPtr &Table::checkItem() const
{
    return d->itemCheck;
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
    assert(false);
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
    return (int)d->period;
}

bool Table::isFrameTable() const
{
    return d->isFrameTable;
}

std::string Table::typeName() const
{
    std::string prefix;
    Icd::Item *parentItem = parent() ? dynamic_cast<Icd::Item *>(parent()) : 0;
    if (parentItem) {
        if (parentItem->type() == Icd::ItemComplex) {
            prefix = "Complex_";
        } else {
            prefix = parentItem->typeName() + "_";
        }
    } else {
        prefix = "Table_";
    }

    return prefix + mark();
}

std::string Table::codeName() const
{
    Icd::Item *parentItem = parent() ? dynamic_cast<Icd::Item *>(parent()) : 0;
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
        memset(d->buffer, 0, (int)bufferSize());
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

Object *Table::clone() const
{
    return new Table(*this);
}

Table &Table::operator =(const Table &other)
{
    Object::operator =(other);

    d->sequence = other.d->sequence;
    d->itemOffset = other.d->itemOffset;
    d->bufferOffset = other.d->bufferOffset;
    d->isFrameTable = other.d->isFrameTable;

    d->bufferSize = 0;
    d->buffer = 0;
    d->items.clear();
    const ItemPtrArray &items = other.d->items;
    for (ItemPtrArray::const_iterator citer = items.cbegin();
         citer != items.cend(); ++citer) {
        const ItemPtr &item = *citer;
        if (item->type() == Icd::ItemFrame) {
            continue;
        }
        appendItem(ItemPtr(reinterpret_cast<Item *>((*citer)->clone())));
    }

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

    assert(deep <= Icd::ObjectItem);

    if (deep <= Icd::ObjectTable) {
        return true;
    }

    if (json.isMember("items")) {
        Json::Value itemsJson = json["items"];
        for (Json::ValueConstIterator citer = itemsJson.begin();
             citer != itemsJson.end(); ++citer) {
            const Json::Value &itemJson = *citer;
            Icd::ItemPtr item = Item::create(Icd::itoa(itemCount() + 1),
                                             Item::stringType(itemJson["type"].asString()));
            if (!item) {
                continue;
            }
            if (!item->restore(itemJson, deep)) {
                continue;
            }
            item->setParent(this);
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
            Icd::FrameItemPtr frame =
                    JHandlePtrCast<Icd::FrameItem, Icd::Item>(itemById(frameId));
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
