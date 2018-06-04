#ifndef ICD_SYSTEM_H
#define ICD_SYSTEM_H

#include "icdcore_global.h"
#include <string>
#include <vector>
#include "icd_table.h"

namespace Icd {

class System;
class SystemData;
typedef std::shared_ptr<System> SystemPtr;
typedef std::vector<SystemPtr> SystemPtrArray;

class Vehicle;
typedef std::shared_ptr<Vehicle> VehiclePtr;

class ICDCORE_EXPORT System : public Object
{
public:
    explicit System(Vehicle *parent = nullptr);
    explicit System(const std::string &id, Vehicle *parent = nullptr);
    ~System();

    TablePtrArray allTable();
    const TablePtrArray &allTable() const;
    void appendTable(const TablePtr &table);
    void insertTable(int index, const TablePtr &table);
    void removeTable(int index);
    void removeTableByMark(const std::string &mark);
    void clearTable();
    int tableCount() const;
    TablePtr tableAt(int index) const;
    TablePtr tableByMark(const std::string &mark) const;

    int childCount() const final;
    void resetData() final;
    void clearData() final;

    Object *clone() const final;
    System &operator =(const System &other);

    // Serializable interface
public:
    Json::Value save() const final;
    bool restore(const Json::Value &json, int deep = -1) final;

protected:
    System(const System &other);

private:
    SystemData *d;
};

} // end of namespace Icd

#endif // ICD_SYSTEM_H
