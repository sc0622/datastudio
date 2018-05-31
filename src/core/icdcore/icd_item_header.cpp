#include "precomp.h"
#include "icd_item_header.h"
#include <sstream>

namespace Icd {

class HeaderItemData
{
    friend class HeaderItem;
public:
    HeaderItemData()
    {

    }

private:
};

HeaderItem::HeaderItem(Object *parent)
    : Item(ItemHead, parent)
    , d(new HeaderItemData())
{
    setBufferSize(1);
}

HeaderItem::HeaderItem(const std::string &id, Object *parent)
    : Item(id, ItemHead, parent)
    , d(new HeaderItemData())
{
    setBufferSize(1);
}

HeaderItem::HeaderItem(const HeaderItem &other)
    : Item(other)
    , d(new HeaderItemData())
{
    operator =(other);
}

HeaderItem::~HeaderItem()
{
    delete d;
}

double HeaderItem::data() const
{
    const char *buffer = this->buffer();
    if (!buffer) {
        return 0.0;
    }

    return *(unsigned char *)buffer;
}

void HeaderItem::setData(double data)
{
    char *buffer = this->buffer();
    if (!buffer) {
        return;
    }

    *(unsigned char *)buffer = (unsigned char)data;
}

std::string HeaderItem::dataString() const
{
    std::stringstream str;
    str<< std::hex << (unsigned char)data();
    return str.str();
}

unsigned char HeaderItem::value() const
{
    return 0;
}

void HeaderItem::setValue(unsigned char value)
{
    (void)value;
}

std::string HeaderItem::typeName() const
{
    return "icd_uint8";
}

Object *HeaderItem::clone() const
{
    return new HeaderItem(*this);
}

HeaderItem &HeaderItem::operator =(const HeaderItem &other)
{
    Item::operator =(other);
    return *this;
}

Json::Value HeaderItem::save() const
{
    Json::Value json = Item::save();
    return json;
}

bool HeaderItem::restore(const Json::Value &json, int deep)
{
    if (!Item::restore(json, deep)) {
        return false;
    }

    // limit mark
    setDefaultValue((int)defaultValue() & 0xff);

    return true;
}

} // end of namespace Icd
