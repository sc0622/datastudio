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
    explicit System(Object *parent = nullptr);
    explicit System(const std::string &id, Object *parent = nullptr);
    System(const System &other);
    ~System() override;

    int rtti() const override;

    TablePtrArray allTable();
    const TablePtrArray &allTable() const;
    void appendTable(const TablePtr &table);
    void insertTable(int index, const TablePtr &table);
    void removeTable(int index);
    void removeTableByMark(const std::string &mark);
    void clearTable();
    int tableCount() const;
    TablePtr tableAt(int index) const;
    TablePtr tableByName(const std::string &name) const;
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
    bool hasChildByName(const std::string &name, const Icd::ObjectPtr &exclude = Icd::ObjectPtr()) const override;
    bool hasChildByMark(const std::string &mark, const Icd::ObjectPtr &exclude = Icd::ObjectPtr()) const override;
    ObjectPtr childAt(icd_uint64 index) const override;
    ObjectPtr replaceChild(icd_uint64 index, ObjectPtr &other) override;
    void moveChild(int sourceIndex, int targetIndex) override;
    void removeChild(icd_uint64 index) override;
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
