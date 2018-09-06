#include "precomp.h"
#include "icd_item_limit.h"
#include <sstream>

namespace Icd {

class LimitItemData
{
    friend class LimitItem;
public:
    LimitItemData()
        : minimum(0.0)
        , maximum(0.0)
        , minimumInf(true)
        , maximumInf(true)
    {

    }

private:
    double minimum;     // 最小值
    double maximum;     // 最大值
    bool minimumInf;    // 是否最小值为-无穷
    bool maximumInf;    // 是否最大值为+无穷
};

LimitItem::LimitItem()
    : Object(ObjectLimit)
    , d(new LimitItemData())
{

}

LimitItem::LimitItem(const LimitItem &other)
    : Object(ObjectLimit)
    , d(new LimitItemData())
{
    operator =(other);
}

LimitItem::~LimitItem()
{
    delete d;
}

double LimitItem::minimum() const
{
    return d->minimum;
}

void LimitItem::setMinimum(double min)
{
    d->minimum = min;
}

double LimitItem::maximum() const
{
    return d->maximum;
}

void LimitItem::setMaximum(double max)
{
    d->maximum = max;
}

bool LimitItem::minimumInf() const
{
    return d->minimumInf;
}

void LimitItem::setMinimumInf(bool inf)
{
    d->minimumInf = inf;
}

bool LimitItem::maximumInf() const
{
    return d->maximumInf;
}

void LimitItem::setMaximumInf(bool inf)
{
    d->maximumInf = inf;
}

LimitItem *LimitItem::clone() const
{
    return new LimitItem(*this);
}

LimitItem &LimitItem::operator =(const LimitItem &other)
{
    d->minimum = other.d->minimum;
    d->maximum = other.d->maximum;
    d->minimumInf = other.d->minimumInf;
    d->maximumInf = other.d->maximumInf;

    return *this;
}

Json::Value LimitItem::save() const
{
    Json::Value json;

    if (!d->minimumInf) {
        json["min"] = minimum();
    }

    if (!d->maximumInf) {
        json["max"] = maximum();
    }

    return json;
}

bool LimitItem::restore(const Json::Value &json, int)
{
    if (json.isMember("min")) {
        setMinimumInf(false);
        setMinimum(json["min"].asDouble());
    } else {
        setMinimumInf(true);
        setMinimum(0);
    }

    if (json.isMember("max")) {
        setMaximumInf(false);
        setMaximum(json["max"].asDouble());
    } else {
        setMaximumInf(true);
        setMaximum(0);
    }

    return true;
}

std::string LimitItem::toString() const
{
    std::ostringstream os;
    os << '[';
    if (d->minimumInf) {
        os << "-Inf";
    } else {
        os << d->minimum;
    }
    os << ", ";
    if (d->maximumInf) {
        os << "Inf";
    } else {
        os << d->maximum;
    }
    os << ']';
    return os.str();
}

} // end of namespace Icd
