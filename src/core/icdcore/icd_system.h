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
    System(const System &other);
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
    bool isEmpty() const;

    int childCount() const override;
    void resetData() override;
    void clearData() override;

    ObjectPtr copy() const override;
    ObjectPtr clone() const override;
    System &operator =(const System &other);

    Icd::ObjectPtr findByDomain(const std::string &domain, int domainType = Icd::DomainId,
                                bool ignoreComplex = true) const override;
    void clearChildren() override;

    // Serializable interface
public:
    Json::Value save() const override;
    bool restore(const Json::Value &json, int deep = -1) override;

private:
    SystemData *d;
};

} // end of namespace Icd

#endif // ICD_SYSTEM_H
