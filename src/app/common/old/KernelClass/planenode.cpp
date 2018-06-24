#include "planenode.h"
#include "systemnode.h"
using std::make_pair;


class PlaneNodePrivate
{
    friend class PlaneNode;
public:
    PlaneNodePrivate() : q_code(-1)
    {}

private:
    int     q_code;    // 机型编号
    // 下挂分系统
    SystemNode::systemVector    q_system;
};

/**
 * @brief 构造函数
 * @param [in] id : 飞机型号码
 * @param [in] name : 飞机名称
 * @param [in] describe : 描述
 */
PlaneNode::PlaneNode(int id,
                     const std::string &name,
                     const std::string &sign,
                     const std::string &describe)
                     : ICDElement(GlobalDefine::ntVehicle)
                     , d(new PlaneNodePrivate())
{
    setNumeralId(id);
    setName(name);
    setSign(sign);
    setDescribe(describe);
}

/**
 * @brief 构造函数
 * @param [in] plane : 飞机基本信息
 */
PlaneNode::PlaneNode(const stPlane &plane)
    : ICDElement(GlobalDefine::ntVehicle)
    , d(new PlaneNodePrivate())
{
    setNumeralId(plane.nCode);
    setName(plane.sName);
    setSign(plane.sSign);
    setDescribe(plane.sDescribe);
}

PlaneNode::~PlaneNode()
{
    if (d) {
        delete d;
        d = NULL;
    }
}

void PlaneNode::setPlane(const stPlane& plane)
{
    setNumeralId(plane.nCode);
    setName(plane.sName);
    setSign(plane.sSign);
    setDescribe(plane.sDescribe);
}

stPlane PlaneNode::plane() const
{
    stPlane plane;

    plane.nCode = numeralId();
    plane.sName = name();
    plane.sSign = sign();
    plane.sDescribe = describe();

    return plane;
}

void PlaneNode::setNumeralId(int id)
{
    char szTmp[32] = {0};
    sprintf_s(szTmp, sizeof(szTmp), "%d", id);

    ICDElement::setId(std::string(szTmp));

    d->q_code = id;
}

int PlaneNode::numeralId() const
{
    return d->q_code;
}

void PlaneNode::setSystem(const SystemNode::systemVector& system)
{
    d->q_system = system;
}

SystemNode::systemVector PlaneNode::allSystem() const
{
    return d->q_system;
}

bool PlaneNode::isEmpty() const
{
    return d->q_system.empty();
}

/**
 * @brief 获取下挂系统信息
 * @param [in] id : 系统标识
 * @return 下挂系统信息
 */
SystemNode::smtSystem PlaneNode::system(int id) const
{
    SystemNode::smtSystem result = 0;
    const int count = d->q_system.size();
    for (int i = 0; i < count; ++i) {
        if ((result = d->q_system[i]) && result->numeralId() == id) {
            break;
        } else {
            result = 0;
        }
    }

    return result;
}

/**
 * @brief 增加下挂分系统
 * @param [in] system : 系统信息集
 */
void PlaneNode::addSystem(const SystemNode::smtSystem &system)
{
    if (!system) {
        return;
    }
    SystemNode::smtSystem _system = 0;
    const int count = d->q_system.size();
    for (int i = 0; i < count; ++i) {
        if ((_system = d->q_system[i]) && _system->id() == system->id()) {
            d->q_system[i] = system;

            break;
        } else {
            _system = 0;
        }
    }
    if (!_system) {
        d->q_system.push_back(system);
    }
}

/**
 * @brief 删除下挂分系统
 * @param [in] system : 系统标识集
 * @return 执行结果，true：成功；false：失败
 */
bool PlaneNode::deleteSystem(const std::vector<int> &system)
{
    const int count(system.size());
    SystemNode::smtSystem _system = 0;
    SystemNode::systemVector::iterator it = d->q_system.begin();
    for (int i = 0; i < count; ++i) {
        for (it = d->q_system.begin(); it != d->q_system.end(); ++it) {
            if ((_system = *it) && _system->numeralId() == system[i]) {
                d->q_system.erase(it);
                break;
            }
        }
    }

    return true;
}

/**
 * @brief 删除下挂所有系统
 */
void PlaneNode::clearSystem()
{
    d->q_system.clear();
}

/**
 * @brief 克隆（深拷贝）
 * @return 机型实例
 */
PlaneNode::smtPlane PlaneNode::clone() const
{
    PlaneNode::smtPlane plane(new PlaneNode(*this));

    return plane;
}

PlaneNode::PlaneNode(const PlaneNode &rhs)
    : ICDElement(rhs)
    , d(new PlaneNodePrivate())
{
    *this = rhs;
}

PlaneNode &PlaneNode::operator =(const PlaneNode &rhs)
{
    if (&rhs == this) {
        return *this;
    }
    *(ICDElement *)this = rhs;
    d->q_code = rhs.d->q_code;

    SystemNode::systemVector cloneSystem;
    const int count = rhs.d->q_system.size();
    for (int i = 0; i < count; ++i) {
        const SystemNode::smtSystem &system = rhs.d->q_system[i];
        if (system) {
            cloneSystem.push_back(system->clone());
        }
    }
    setSystem(cloneSystem);

    return *this;
}
