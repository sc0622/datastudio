#ifndef ICD_SYSTEM_H
#define ICD_SYSTEM_H

#include "icdcore_global.h"
#include "icd_object.h"
#include <string>
#include <vector>
#include "icd_table.h"

namespace Icd {

class System;
class SystemData;
typedef JHandlePtr<System> SystemPtr;
typedef std::vector<SystemPtr> SystemPtrArray;

class Vehicle;
typedef JHandlePtr<Vehicle> VehiclePtr;

/**
 * @brief 飞机子系统类
 */
class ICDCORE_EXPORT System : public Object
{
public:
    /**
     * @brief System
     * @param parent
     */
    explicit System(Vehicle *parent = 0);

    /**
     * @brief System
     * @param id
     * @param parent
     */
    explicit System(const std::string &id, Vehicle *parent = 0);

    virtual ~System();

    /**
     * @brief 获取系统中的ICD表列表
     * @return
     */
    TablePtrArray allTable();

    /**
     * @brief allTable
     * @return
     */
    const TablePtrArray &allTable() const;

    /**
     * @brief 添加ICD表
     * @param [in] icdTable : 待添加的ICD表
     */
    void appendTable(const TablePtr &table);

    /**
     * @brief 插入ICD表
     * @param [int] index : 插入位置
     * @param [in] icdTable : 待插入的ICD表
     */
    void insertTable(int index, const TablePtr &table);

    /**
     * @brief 删除指定位置的ICD表
     * @param [in] index : 待删除ICD表的位置
     */
    void removeTable(int index);

    /**
     * @brief 删除指定标识的ICD表
     * @param [in] mark : 待删除ICD表的标识
     */
    void removeTableByMark(const std::string &mark);

    /**
     * @brief 清空所有ICD表
     */
    void clearTable();

    /**
     * @brief tableCount
     * @return
     */
    int tableCount() const;

    /**
     * @brief tableAt
     * @param index
     * @return
     */
    TablePtr tableAt(int index) const;

    /**
     * @brief tableByMark
     * @param mark
     * @return
     */
    TablePtr tableByMark(const std::string &mark) const;

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
    System &operator =(const System &other);

    // Serializable interface
public:
    Json::Value save() const;
    bool restore(const Json::Value &json, int deep = -1);

protected:
    /**
     * @brief System
     * @param other
     */
    System(const System &other);

private:
    SystemData *d;
};

} // end of namespace Icd

#endif // ICD_SYSTEM_H
