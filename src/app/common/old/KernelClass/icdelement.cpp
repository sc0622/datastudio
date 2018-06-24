#include "icdelement.h"


class ICDElementPrivate
{
    friend class ICDElement;
public:
    ICDElementPrivate() : objectType(GlobalDefine::ntUnknown)
    {}

private:
    GlobalDefine::NodeType  objectType; // 数据类型
    std::string id;     // 编号
    std::string name;   // 名称
    std::string sign;   // 标识，需满足C语言命名规范,同一ICD表中唯一
    std::string desc;   // 描述
};

/**
* @brief 构造函数
* @param [in] type : 数据类型
*/
ICDElement::ICDElement(GlobalDefine::NodeType type)
                     : d(new ICDElementPrivate())
{
    d->objectType = type;
}

ICDElement::~ICDElement()
{
    if (d) {
        delete d;
        d = NULL;
    }
}

GlobalDefine::NodeType ICDElement::objectType() const
{
    return d->objectType;
}

void ICDElement::setId(std::string id)
{
    d->id = id;
}

std::string ICDElement::id() const
{
    return d->id;
}

void ICDElement::setName(const std::string& name)
{
    d->name = name;
}

std::string ICDElement::name() const
{
    return d->name;
}

void ICDElement::setProCode(const std::string &proCode)
{
    d->sign = proCode;
}

std::string ICDElement::proCode() const
{
    return d->sign;
}

void ICDElement::setSign(const std::string& sign)
{
    d->sign = sign;
}

std::string ICDElement::sign() const
{
    return d->sign;
}

void ICDElement::setDescribe(const std::string& describe)
{
    d->desc = describe;
}

std::string ICDElement::describe() const
{
    return d->desc;
}

/**
 * @brief 克隆（深拷贝）
 * @return 机型实例
 */
ICDElement::smtElement ICDElement::clone() const
{
    ICDElement::smtElement element(new ICDElement(*this));

    return element;
}

ICDElement::ICDElement(const ICDElement &rhs)
    : d(new ICDElementPrivate())
{
    *this = rhs;
}

ICDElement &ICDElement::operator =(const ICDElement &rhs)
{
    if (&rhs == this) {
        return *this;
    }
    d->objectType = rhs.d->objectType;
    d->id = rhs.d->id;
    d->name = rhs.d->name;
    d->sign = rhs.d->sign;
    d->desc = rhs.d->desc;

    return *this;
}

std::string ICDElement::typeString() const
{
    switch (d->objectType) {
    case GlobalDefine::ntVehicle: return "vehicle";
    case GlobalDefine::ntSystem: return "system";
    case GlobalDefine::ntTable: return "table";
    case GlobalDefine::ntRule: return "item";
    default: return "?";
    }
}
