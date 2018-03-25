//#include "precomp.h"
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
        , leftInf(true)
        , rightInf(true)
    {

    }

private:
    double minimum;     // 最小值
    double maximum;     // 最大值
    bool leftInf;       // 是否最小值为-无穷
    bool rightInf;      // 是否最大值为+无穷
};

LimitItem::LimitItem()
    : d(new LimitItemData())
{

}

LimitItem::LimitItem(const LimitItem &other)
    : d(new LimitItemData())
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

bool LimitItem::leftInf() const
{
    return d->leftInf;
}

void LimitItem::setLeftInf(bool leftInf)
{
    d->leftInf = leftInf;
}

bool LimitItem::rightInf() const
{
    return d->rightInf;
}

void LimitItem::setRightInf(bool rightInf)
{
    d->rightInf = rightInf;
}

LimitItem *LimitItem::clone() const
{
    return new LimitItem(*this);
}

LimitItem &LimitItem::operator =(const LimitItem &other)
{
    d->minimum = other.d->minimum;
    d->maximum = other.d->maximum;
    d->leftInf = other.d->leftInf;
    d->rightInf = other.d->rightInf;

    return *this;
}

Json::Value LimitItem::save() const
{
    Json::Value json;

    if (!d->leftInf) {
        json["min"] = minimum();
    }

    if (!d->rightInf) {
        json["max"] = maximum();
    }

    return json;
}

bool LimitItem::restore(const Json::Value &json, int)
{
    if (json.isMember("min")) {
        setLeftInf(false);
        setMinimum(json["min"].asDouble());
    } else {
        setLeftInf(true);
        setMinimum(0);
    }

    if (json.isMember("max")) {
        setRightInf(false);
        setMaximum(json["max"].asDouble());
    } else {
        setRightInf(true);
        setMaximum(0);
    }

    return true;
}

std::string LimitItem::toString() const
{
    std::ostringstream os;
    os << '[';
    if (d->leftInf) {
        os << "-Inf";
    } else {
        os << d->minimum;
    }
    os << ", ";
    if (d->rightInf) {
        os << "Inf";
    } else {
        os << d->maximum;
    }
    os << ']';
    return os.str();
}

} // end of namespace Icd
