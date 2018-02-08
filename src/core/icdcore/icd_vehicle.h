#ifndef ICD_VEHICLE_H
#define ICD_VEHICLE_H

#include "icdcore_global.h"
#include <string>
#include <vector>
#include "icd_system.h"

namespace Icd {

class Vehicle;
class VehicleData;
typedef JHandlePtr<Vehicle> VehiclePtr;
typedef std::vector<VehiclePtr> VehiclePtrArray;

/**
 * @brief 飞机全机类
 */
class ICDCORE_EXPORT Vehicle : public Object
{
public:
    /**
     * @brief Vehicle
     * @param parent
     */
    explicit Vehicle(Object *parent = 0);

    /**
     * @brief Vehicle
     * @param id
     * @param parent
     */
    explicit Vehicle(const std::string &id, Object *parent = 0);

    ~Vehicle();

    /**
     * @brief 获取飞机系统列表
     * @return
     */
    SystemPtrArray allSystem();

    /**
     * @brief 获取飞机系统列表
     * @return
     */
    const SystemPtrArray &allSystem() const;

    /**
     * @brief 添加新系统到系统列表
     * @param [in] system : 待添加系统
     */
    void appendSystem(const SystemPtr &system);

    /**
     * @brief 插入新系统到系统列表
     * @param [in] index : 系统列表索引号
     * @param [in] system ： 待插入系统
     */
    void insertSystem(int index, const SystemPtr &system);

    /**
     * @brief 删除系统
     * @param [in] index : 待删除系统索引号
     */
    void removeSystem(int index);

    /**
     * @brief 删除系统
     * @param [in] mark : 待删除系统标识
     */
    void removeSystemByMark(const std::string &mark);

    /**
     * @brief 情况系统列表
     */
    void clearSystem();

    /**
     * @brief systemCount
     * @return
     */
    int systemCount() const;

    /**
     * @brief 获取指定位置的系统对象
     * @param [in] index : 系统在列表中的位置
     * @return
     */
    SystemPtr systemAt(int index) const;

    /**
     * @brief 获取指定标识的系统对象
     * @param [in] mark : 系统标识
     * @return
     */
    SystemPtr systemByMark(const std::string &mark) const;

    /**
     * @brief childCount
     * @return
     */
    int childCount() const;

    /**
     * @brief resetData
     */
    void resetData();

    /**
     * @brief clearData
     */
    void clearData();

    /**
     * @brief clone
     * @return.
     */
    Object *clone() const;

    /**
     * @brief operator =
     * @param other
     * @return
     */
    Vehicle &operator =(const Vehicle &other);

    // Serializable interface
public:
    Json::Value save() const;
    bool restore(const Json::Value &json, int deep = -1);

protected:
    /**
     * @brief Vehicle
     * @param other
     */
    Vehicle(const Vehicle &other);

private:
    VehicleData *d;
};

} // end of namespace Icd

#endif // ICD_VEHICLE_H
