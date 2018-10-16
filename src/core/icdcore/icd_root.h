#ifndef ICD_ROOT_H
#define ICD_ROOT_H

#include "icdcore_global.h"
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
    Root(const Root &other);
    ~Root() override;

    int rtti() const override;

    VehiclePtrArray allVehicle();
    const VehiclePtrArray &allVehicle() const;

    void appendVehicle(const VehiclePtr &vehicle);
    void insertVehicle(int index, const VehiclePtr &vehicle);
    void removeVehicle(int index);
    void removeVehicleByMark(const std::string &mark);
    void clearVehicle();
    int vehicleCount() const;
    VehiclePtr vehicleAt(int index) const;
    VehiclePtr vehicleByName(const std::string &name) const;
    VehiclePtr vehicleByMark(const std::string &mark) const;
    bool isEmpty() const;

    int childCount() const override;
    void resetData() override;
    void clearData() override;

    ObjectPtr copy() const override;
    ObjectPtr clone() const override;
    Root &operator =(const Root &other);

    Icd::ObjectPtr findByDomain(const std::string &domain, int domainType = Icd::DomainId,
                                bool ignoreComplex = true) const override;
    bool hasChildByName(const std::string &name, const Icd::ObjectPtr &exclude = Icd::ObjectPtr()) const override;
    bool hasChildByMark(const std::string &mark, const Icd::ObjectPtr &exclude = Icd::ObjectPtr()) const override;
    ObjectPtr childAt(icd_uint64 index) const override;
    ObjectPtr replaceChild(icd_uint64 index, ObjectPtr &other) override;
    ObjectPtr replaceChild(const std::string &id, ObjectPtr &other) override;
    void moveChild(int sourceIndex, int targetIndex) override;
    void removeChild(icd_uint64 beginIndex, int endIndex = -1) override;
    void removeChild(const std::list<icd_uint64> &indexes) override;
    void clearChildren() override;

    // Serializable interface
public:
    Json::Value save() const override;
    bool restore(const Json::Value &json, int deep = -1) override;

private:
    RootData *d;
};

} // end of namespace Icd

#endif // ICD_ROOT_H
