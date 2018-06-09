#include "precomp.h"
#include "icd_item_datetime.h"

namespace Icd {

// class DateTimeItemData

class DateTimeItemData
{
    friend class DateTimeItem;
public:
    DateTimeItemData()
    {

    }

private:
};

// class DateTimeItem

DateTimeItem::DateTimeItem(Object *parent)
    : Item(ItemDateTime, parent)
    , d(new DateTimeItemData())
{

}

DateTimeItem::DateTimeItem(const std::string &id, Object *parent)
    : Item(id, ItemDateTime, parent)
    , d(new DateTimeItemData())
{

}

DateTimeItem::DateTimeItem(const DateTimeItem &other)
    : Item(other)
    , d(new DateTimeItemData())
{
    operator =(other);
}

DateTimeItem::~DateTimeItem()
{
    delete d;
}

std::string DateTimeItem::typeName() const
{
    std::stringstream ss;
    //TODO
    return ss.str();
}

Object *DateTimeItem::clone() const
{
    return new DateTimeItem(*this);
}

DateTimeItem &DateTimeItem::operator =(const DateTimeItem &other)
{
    Item::operator =(other);
    //TODO
    return *this;
}

Json::Value DateTimeItem::save() const
{
    Json::Value json = Item::save();
    //TODO
    return json;
}

bool DateTimeItem::restore(const Json::Value &json, int deep)
{
    if (!Item::restore(json, deep)) {
        return false;
    }
    //TODO
}

}
