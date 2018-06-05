#ifndef ICD_VEHICLE_H
#define ICD_VEHICLE_H

#include "icdcore_global.h"
#include <string>
#include <vector>
#include "icd_system.h"

namespace Icd {

class Vehicle;
class VehicleData;
typedef std::shared_ptr<Vehicle> VehiclePtr;
typedef std::vector<VehiclePtr> VehiclePtrArray;

class ICDCORE_EXPORT Vehicle : public Object
{
public:
    explicit Vehicle(Object *parent = nullptr);
    explicit Vehicle(const std::string &id, Object *parent = nullptr);
    ~Vehicle();

    SystemPtrArray allSystem();
    const SystemPtrArray &allSystem() const;
    void appendSystem(const SystemPtr &system);
    void insertSystem(int index, const SystemPtr &system);
    void removeSystem(int index);
    void removeSystemByMark(const std::string &mark);
    void clearSystem();
    int systemCount() const;
    SystemPtr systemAt(int index) const;
    SystemPtr systemByMark(const std::string &mark) const;

    int childCount() const override;
    void resetData() override;
    void clearData() override;

    Object *clone() const override;
    Vehicle &operator =(const Vehicle &other);

    // Serializable interface
public:
    Json::Value save() const override;
    bool restore(const Json::Value &json, int deep = -1) override;

protected:
    Vehicle(const Vehicle &other);

private:
    VehicleData *d;
};

} // end of namespace Icd

#endif // ICD_VEHICLE_H
