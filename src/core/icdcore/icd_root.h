﻿#ifndef ICD_ROOT_H
#define ICD_ROOT_H

#include "icdcore_global.h"
#include <string>
#include <vector>
#include "icd_vehicle.h"

namespace Icd {

class Root;
class RootData;
typedef std::shared_ptr<Root> RootPtr;

class ICDCORE_EXPORT Root : public Object
{
public:
    explicit Root(Object *parent = nullptr);
    explicit Root(const std::string &id, Object *parent = nullptr);
    ~Root();

    VehiclePtrArray allVehicle();
    const VehiclePtrArray &allVehicle() const;

    void appendVehicle(const VehiclePtr &vehicle);
    void insertVehicle(int index, const VehiclePtr &vehicle);
    void removeVehicle(int index);
    void removeVehicleByMark(const std::string &mark);

    void clearVehicle();
    int vehicleCount() const;
    VehiclePtr vehicleAt(int index) const;
    VehiclePtr vehicleByMark(const std::string &mark) const;
    int childCount() const override;
    void resetData() override;
    void clearData() override;
    Object *clone() const override;
    Root &operator =(const Root &other);

    // Serializable interface
public:
    Json::Value save() const override;
    bool restore(const Json::Value &json, int deep = -1) override;

protected:
    Root(const Root &other);

private:
    RootData *d;
};

} // end of namespace Icd

#endif // ICD_ROOT_H
