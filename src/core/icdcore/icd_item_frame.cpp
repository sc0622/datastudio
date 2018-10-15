#include "precomp.h"
#include "icd_item_frame.h"
#include "icd_item_framecode.h"
#include "icd_table.h"
#include <algorithm>
#include <unordered_map>

namespace Icd {

//
typedef std::unordered_multimap<icd_uint64/*sequence*/, TablePtr> TableSeq;
typedef std::pair<TableSeq::const_iterator/*cbegin*/, TableSeq::const_iterator/*cend*/> TableSeqPair;
typedef std::unordered_map<int/*sequence*/, TableSeq::const_iterator> TableSeqIterMap;

// class FrameItemData

class FrameItemData
{
    friend class FrameItem;
public:
    FrameItemData()
        : sequenceCount(1)
        , currentSequence(1)
    {

    }

    void clearBuffer();
    void updateSend(FrameItem *frame, const TablePtr &table, bool period);

private:
    int sequenceCount;
    int currentSequence;
    TablePtrMap tables;
    TableSeq tableSeq;
    TableSeqIterMap seqIterMap;
};

void FrameItemData::clearBuffer()
{
    TablePtrMap::const_iterator citer = tables.begin();
    for (; citer != tables.end(); ++citer) {
        char* buffer = citer->second->buffer();
        if (buffer) {
            citer->second->setBuffer(nullptr);
            delete[] buffer;
        }
    }
}

void FrameItemData::updateSend(FrameItem *frame, const TablePtr &table, bool period)
{
    //
    table->updateSend(period);

    //
    char* frameBuffer = frame->buffer();
    char* tableBuffer = table->buffer();
    if (frameBuffer && tableBuffer) {
        const int frameSize = frame->bufferSize();
        const int tableSize = table->bufferSize();
        const int size = std::min(frameSize, tableSize);
        memcpy(frameBuffer, tableBuffer, size_t(size));
        if (frameSize > tableSize) {
            memset(frameBuffer + tableSize, 0, size_t(frameSize - tableSize));
        }
    }
}

// class FrameItem

FrameItem::FrameItem(Object *parent)
    : Item(ItemFrame, parent)
    , d(new FrameItemData())
{

}

FrameItem::FrameItem(const std::string &id, Object *parent)
    : Item(id, ItemFrame, parent)
    , d(new FrameItemData())
{

}

FrameItem::FrameItem(const FrameItem &other)
    : Item(other)
    , d(new FrameItemData())
{
    operator =(other);
}

void FrameItem::setBufferOffset(int offset)
{
    Item::setBufferOffset(offset);

    for (Icd::TablePtrMap::const_iterator citer = d->tables.cbegin();
         citer != d->tables.cend(); ++citer) {
        citer->second->setBufferOffset(offset);
    }
}

void FrameItem::adjustBufferOffset()
{
    const int offset = bufferOffset();
    for (Icd::TablePtrMap::const_iterator citer = d->tables.cbegin();
         citer != d->tables.cend(); ++citer) {
        citer->second->setBufferOffset(offset);
    }
}

FrameItem::~FrameItem()
{
    d->clearBuffer();

    delete d;
}

int FrameItem::rtti() const
{
    return ObjectFrame;
}

bool FrameItem::isSimpleItem() const
{
    return false;
}

void FrameItem::addTable(const TablePtr &table)
{
    if (!table) {
        return;
    }

    const icd_uint64 code = table->frameCode();
    TablePtrMap::const_iterator citer = d->tables.find(code);
    if (citer != d->tables.end()) {
        return;
    }

    d->tables[code] = table;
    d->tableSeq.insert(std::pair<icd_uint64, TablePtr>(table->sequence(), table));

    table->setBufferOffset(bufferOffset());

    if (bufferSize() < table->bufferSize()) {
        setBufferSize(table->bufferSize());
    }
}

void FrameItem::removeTable(icd_uint64 code)
{
    TablePtrMap::const_iterator citer = d->tables.find(code);
    if (citer != d->tables.find(code)) {
        const TablePtr &table = citer->second;
        char* buffer = table->buffer();
        if (buffer) {
            delete[] buffer;
        }
        d->tables.erase(citer);
        //
        const TableSeqPair tableSeqs = d->tableSeq.equal_range(static_cast<size_t>(table->sequence()));
        TableSeq::const_iterator citerSeq = tableSeqs.first;
        for (; citerSeq != tableSeqs.second; ++citerSeq) {
            if (citerSeq->second == table) {
                d->tableSeq.erase(citerSeq);
            }
        }
    }
}

void FrameItem::clearTable()
{
    d->tables.clear();
}

bool FrameItem::replaceCode(icd_uint64 oldCode, icd_uint64 newCode)
{
    if (oldCode == newCode) {
        return true;
    }

    if (d->tables.find(newCode) != d->tables.end()) {
        return false;
    }

    TablePtrMap::const_iterator citerOld = d->tables.find(oldCode);
    if (citerOld == d->tables.end()) {
        return false;
    }

    d->tables[newCode] = citerOld->second;
    d->tables.erase(citerOld);

    return true;
}

TablePtrMap FrameItem::allTable()
{
    return d->tables;
}

const TablePtrMap &FrameItem::allTable() const
{
    return d->tables;
}

int FrameItem::tableCount() const
{
    return static_cast<int>(d->tables.size());
}

TablePtr FrameItem::tableAt(icd_uint64 code) const
{
    TablePtrMap::const_iterator citer = d->tables.find(code);
    if (citer != d->tables.end()) {
        return citer->second;
    } else {
        return TablePtr();
    }
}

TablePtr FrameItem::tableById(const std::string &id) const
{
    for (TablePtrMap::const_iterator citer = d->tables.cbegin();
         citer != d->tables.cend(); ++citer) {
        const TablePtr &table = citer->second;
        if (table->id() == id) {
            return table;
        }
    }

    return TablePtr();
}

std::string FrameItem::dataString() const
{
    return "";
}

void FrameItem::setBuffer(char *buffer)
{
    Item::setBuffer(buffer);

    if (buffer) {
        TablePtrMap::iterator iter = d->tables.begin();
        for (; iter != d->tables.end(); ++iter) {
            TablePtr &table = iter->second;
            size_t tableSize = size_t(table->bufferSize());
            if (!table->buffer() && tableSize > 0) {
                char *tableBuffer = new char[tableSize];
                memset(tableBuffer, 0, tableSize);
                table->setBuffer(tableBuffer);
            }
        }
    } else {
        d->clearBuffer();
    }
}

void FrameItem::setBufferSize(int size)
{
    Item::setBufferSize(size);
}

int FrameItem::sequenceCount() const
{
    return d->sequenceCount;
}

void FrameItem::setSequenceCount(int count)
{
    d->sequenceCount = count;
}

std::string FrameItem::typeName() const
{
    return "Frame_" + codeName();
}

int FrameItem::childCount() const
{
    int count = 0;

    // children
    for (TablePtrMap::const_iterator citer = d->tables.cbegin();
         citer != d->tables.end(); ++citer) {
        count += citer->second->childCount();
    }

    return count;
}

void FrameItem::resetData()
{
    for (TablePtrMap::const_iterator citer = d->tables.cbegin();
         citer != d->tables.end(); ++citer) {
        citer->second->resetData();
    }
}

void FrameItem::clearData()
{
    for (TablePtrMap::const_iterator citer = d->tables.cbegin();
         citer != d->tables.end(); ++citer) {
        citer->second->clearData();
    }
}

ObjectPtr FrameItem::copy() const
{
    FrameItemPtr newFrame = std::make_shared<FrameItem>(*this);
    return newFrame;
}

ObjectPtr FrameItem::clone() const
{
    FrameItemPtr newFrame = std::make_shared<FrameItem>(*this);
    // children
    const TablePtrMap &tables = d->tables;
    TablePtrMap::const_iterator citer = tables.cbegin();
    for (; citer != tables.cend(); ++citer) {
        TablePtr newTable = JHandlePtrCast<Table>(citer->second->clone());
        newTable->setParent(newFrame.get());
        newFrame->addTable(newTable);
    }
    return newFrame;
}

FrameItem &FrameItem::operator =(const FrameItem &other)
{
    if (this == &other) {
        return *this;
    }
    Item::operator =(other);
    d->sequenceCount = other.d->sequenceCount;
    d->currentSequence = other.d->currentSequence;
    d->tables = other.allTable();
    return *this;
}

ObjectPtr FrameItem::itemByName(const std::string &name, bool deep) const
{
    for (TablePtrMap::const_iterator citer = d->tables.cbegin();
         citer != d->tables.end(); ++citer) {
        const TablePtr &table = citer->second;
        if (table->name() == name) {
            return table;
        } else if (deep) {
            const ObjectPtr childItem = table->itemByName(name, deep);
            if (childItem) {
                return childItem;
            }
        }
    }

    return ObjectPtr();
}

ObjectPtr FrameItem::itemByMark(const std::string &mark, bool deep) const
{
    for (TablePtrMap::const_iterator citer = d->tables.cbegin();
         citer != d->tables.end(); ++citer) {
        const TablePtr &table = citer->second;
        if (table->mark() == mark) {
            return table;
        } else if (deep) {
            const ObjectPtr childItem = table->itemByMark(mark, deep);
            if (childItem) {
                return childItem;
            }
        }
    }

    return ObjectPtr();
}

TablePtr FrameItem::tableByMark(const std::string &mark, bool deep) const
{
    for (TablePtrMap::const_iterator citer = d->tables.cbegin();
         citer != d->tables.end(); ++citer) {
        const TablePtr &table = citer->second;
        if (table->mark() == mark) {
            return table;
        } else if (deep) {
            const TablePtr childTable = table->tableByMark(mark, deep);
            if (childTable) {
                return childTable;
            }
        }
    }

    return TablePtr();
}

ObjectPtr FrameItem::itemByDomain(const std::string &domain, DomainType domainType,
                                  bool ignoreComplex) const
{
    if (domain.empty()) {
        return ItemPtr();
    }

    std::string::size_type index = domain.find_first_of('/');
    std::string current, next;
    if (index == std::string::npos) {
        current = domain;
    } else {
        current = domain.substr(0, index);
        next = domain.substr(index + 1);
    }

    for (Icd::TablePtrMap::const_iterator citer = d->tables.cbegin();
         citer != d->tables.cend(); ++citer) {
        const TablePtr &table = citer->second;
        if (table->domainOfType(domainType) != current) {
            continue;
        }

        if (next.empty()) {
            return table;
        }

        const ObjectPtr childItem = table->itemByDomain(next, domainType, ignoreComplex);
        if (childItem) {
            return childItem;
        }

        break;
    }

    return Icd::ItemPtr();
}

TablePtr FrameItem::tableByDomain(const std::string &domain, DomainType domainType) const
{
    ObjectPtr item = itemByDomain(domain, domainType);
    if (!item || item->objectType() != ObjectTable) {
        return TablePtr();
    }

    return JHandlePtrCast<Table>(item);
}

icd_uint64 FrameItem::updateSend(icd_uint64 code)
{
    if (code > 0) {
        size_t size = d->tables.size();
        if (size == 1) {
            const TablePtr &table = d->tables.cbegin()->second;
            if (table) {
                table->updateSend(false);
            }
        } else if (size > 1) {
            TablePtrMap::const_iterator citer = d->tables.find(code);
            if (citer != d->tables.end()) {
                const TablePtr &table = citer->second;
                if (table) {
                    d->updateSend(this, table, false);
                }
            }
        }
    } else {
        TableSeqPair tableSeqs = d->tableSeq.equal_range(static_cast<size_t>(d->currentSequence));
        if (tableSeqs.first != tableSeqs.second) {	// not at end
            TableSeq::const_iterator citer = tableSeqs.first;
            Icd::TablePtr table = citer->second;
            if (++citer != tableSeqs.second) {		// one more elements
                TableSeqIterMap::iterator iterSeq = d->seqIterMap.find(d->currentSequence);
                if (iterSeq == d->seqIterMap.cend()) {
                    d->seqIterMap.insert(std::make_pair(d->currentSequence, tableSeqs.first));
                } else {
                    ++iterSeq->second;
                    if (iterSeq->second == tableSeqs.second) {
                        iterSeq->second = tableSeqs.first;
                    }
                    table = iterSeq->second->second;
                }
            }

            if (table) {
                d->updateSend(this, table, true);
                code = table->frameCode();
            }
        }

        ++d->currentSequence;
        if (d->currentSequence > d->sequenceCount) {
            d->currentSequence = 1;
        }
    }

    return code;
}

ObjectPtr FrameItem::findByDomain(const std::string &domain, int domainType,
                                  bool ignoreComplex) const
{
    return itemByDomain(domain, Icd::DomainType(domainType), ignoreComplex);
}

bool FrameItem::hasChildByName(const std::string &name, const Icd::ObjectPtr &exclude) const
{
    for (TablePtrMap::const_iterator citer = d->tables.cbegin();
         citer != d->tables.cend(); ++citer) {
        const TablePtr &table = citer->second;
        if (exclude && table->id() == exclude->id()) {
            continue;
        }
        if (table->name() == name) {
            return true;
        }
    }

    return false;
}

bool FrameItem::hasChildByMark(const std::string &mark, const Icd::ObjectPtr &exclude) const
{
    for (TablePtrMap::const_iterator citer = d->tables.cbegin();
         citer != d->tables.cend(); ++citer) {
        const TablePtr &table = citer->second;
        if (exclude && table->id() == exclude->id()) {
            continue;
        }
        if (table->mark() == mark) {
            return true;
        }
    }

    return false;
}

ObjectPtr FrameItem::childAt(icd_uint64 index) const
{
    return tableAt(index);
}

ObjectPtr FrameItem::replaceChild(icd_uint64 index, ObjectPtr &other)
{
    if (!other || other->objectType() != ObjectTable) {
        return ObjectPtr();
    }

    TablePtrMap::iterator iter = d->tables.find(index);
    if (iter == d->tables.end()) {
        return ObjectPtr();
    }

    TablePtr old = iter->second;
    if (old == other) {
        iter->second = JHandlePtrCast<Table>(other);
        // children
        const ItemPtrArray &items = iter->second->allItem();
        ItemPtrArray::const_iterator citer = items.cbegin();
        for (; citer != items.cend(); ++citer) {
            const ItemPtr &item = *citer;
            item->setParent(iter->second.get());
        }
    }

    return old;
}

ObjectPtr FrameItem::replaceChild(const std::string &id, ObjectPtr &other)
{
    if (!other || other->objectType() != Icd::ObjectTable) {
        return ObjectPtr();
    }

    const Icd::TablePtr otherTable = JHandlePtrCast<Icd::Table>(other);
    if (!otherTable) {
        return ObjectPtr();
    }

    const Icd::icd_uint64 frameCode = otherTable->frameCode();

    for (TablePtrMap::iterator iter = d->tables.begin();
         iter != d->tables.end(); ++iter) {
        const TablePtr &table = iter->second;
        if (table->id() != id) {
            continue;
        }
        if (iter->first != frameCode) {
            d->tables.erase(iter);
            d->tables[frameCode] = otherTable;
        } else {
            iter->second = otherTable;
        }
        return table;
    }

    return ObjectPtr();
}

void FrameItem::removeChild(icd_uint64 beginIndex, int endIndex)
{
    (void)endIndex;
    TablePtrMap::iterator iter = d->tables.find(beginIndex);
    if (iter == d->tables.end()) {
        return;
    }

    d->tables.erase(iter);
}

void FrameItem::removeChild(const std::list<icd_uint64> &indexes)
{
    for (std::list<icd_uint64>::const_iterator citer = indexes.cbegin();
         citer != indexes.cend(); ++citer) {
        const icd_uint64 index = *citer;
        removeChild(index, -1);
    }
}

void FrameItem::clearChildren()
{
    d->tables.clear();
}

void FrameItem::updateRecv(icd_uint64 code)
{
    TablePtrMap::const_iterator citer = d->tables.find(code);
    if (citer == d->tables.end()) {
        return;
    }

    const TablePtr &table = citer->second;
    if (table) {
        //
        char* frameBuffer = buffer();
        char* tableBuffer = table->buffer();
        if (frameBuffer && tableBuffer) {
            const int frameSize = bufferSize();
            const int tableSize = table->bufferSize();
            const int size = std::min(frameSize, tableSize);
            memcpy(tableBuffer, frameBuffer, size_t(size));
        }
        //
        table->updateRecv();
    }
}

void FrameItem::resetSend()
{
    d->currentSequence = 1;
    d->seqIterMap.clear();
}

Json::Value FrameItem::save() const
{
    Json::Value json = Item::save();

    json["sequenceCount"] = sequenceCount();

    if (!d->tables.empty()) {
        Json::Value tablesJson(Json::arrayValue);
        for (TablePtrMap::const_iterator citer = d->tables.cbegin();
             citer != d->tables.cend(); ++citer) {
            const TablePtr &table = citer->second;
            if (table) {
                tablesJson.append(table->save());
            }
        }
        json["tables"] = tablesJson;
    }

    return json;
}

bool FrameItem::restore(const Json::Value &json, int deep)
{
    if (!Item::restore(json, deep)) {
        return false;
    }

    setSequenceCount(json["sequenceCount"].asInt());

    clearTable();
    if (json.isMember("tables")) {
        Json::Value tablesJson = json["tables"];
        for (Json::ValueConstIterator citer = tablesJson.begin();
             citer != tablesJson.end(); ++citer) {
            const Json::Value &tableJson = *citer;
            Icd::TablePtr table = std::make_shared<Icd::Table>(tableJson["id"].asString(), this);
            if (!table->restore(tableJson, deep)) {
                continue;
            }
            addTable(table);
        }
    }

    return true;
}

} // end of namespace Icd
