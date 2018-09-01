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
    return dataFromBuffer(buffer());
}

void HeaderItem::setData(double data)
{
    char *buffer = this->buffer();
    if (!buffer) {
        return;
    }

    *reinterpret_cast<unsigned char*>(buffer) = static_cast<unsigned char>(data);
}

std::string HeaderItem::dataString() const
{
    std::stringstream ss;
    ss << std::hex << static_cast<unsigned char>(data());
    return ss.str();
}

double HeaderItem::dataFromBuffer(const char *buffer) const
{
    if (!buffer) {
        return 0.0;
    }

    return *reinterpret_cast<const unsigned char*>(buffer);
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
    setDefaultValue(static_cast<int>(defaultValue()) & 0xff);

    return true;
}

} // end of namespace Icd
