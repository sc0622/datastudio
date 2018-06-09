#include "precomp.h"
#include "icd_item_string.h"

namespace Icd {

// class StringItemData

class StringItemData
{
    friend class StringItem;
public:
    StringItemData()
    {

    }

private:
};

// class StringItem

StringItem::StringItem(Object *parent)
    : Item(ItemString, parent)
    , d(new StringItemData())
{

}

StringItem::StringItem(const std::string &id, Object *parent)
    : Item(id, ItemString, parent)
    , d(new StringItemData())
{

}

StringItem::StringItem(const StringItem &other)
    : Item(other)
    , d(new StringItemData())
{
    operator =(other);
}

StringItem::~StringItem()
{
    delete d;
}

std::string StringItem::typeName() const
{
    std::stringstream ss;
    ss << "char[" << int(bufferSize()) << ']';
    return ss.str();
}

Object *StringItem::clone() const
{
    return new StringItem(*this);
}

StringItem &StringItem::operator =(const StringItem &other)
{
    Item::operator =(other);
    //TODO
    return *this;
}

Json::Value StringItem::save() const
{
    Json::Value json = Item::save();
    //TODO
    return json;
}

bool StringItem::restore(const Json::Value &json, int deep)
{
    if (!Item::restore(json, deep)) {
        return false;
    }
    //TODO
    return true;
}

}
