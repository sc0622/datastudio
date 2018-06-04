#ifndef ICD_OBJECT_H
#define ICD_OBJECT_H

#include "icdcore_global.h"
#include "./3rdpart/jsoncpp/json_tool.h"

namespace Icd {

//
enum ObjectType {
    ObjectInvalid = -1,
    ObjectRoot,
    ObjectVehicle,
    ObjectSystem,
    ObjectTable,
    ObjectItem,
    ObjectLimit
};

//
enum DomainType {
    DomainValid = -1,
    DomainId,
    DomainName,
    DomainMark
};

class Object;
class ObjectData;
typedef std::shared_ptr<Object> ObjectPtr;

class ICDCORE_EXPORT Object : public Json::Serializable
{
public:
    explicit Object(ObjectType type = ObjectInvalid, Object *parent = nullptr);
    explicit Object(const std::string &id, ObjectType type = ObjectInvalid,
                    Object *parent = nullptr);
    virtual ~Object();

    ObjectType objectType() const;
    std::string id() const;
    std::string domain() const;
    void setDomain(const std::string &domain);
    void setId(const std::string &id);
    std::string name() const;
    void setName(const std::string &name);
    std::string mark() const;
    void setMark(const std::string &mark);
    std::string desc() const;
    void setDesc(const std::string &desc);

    virtual bool isPrivateMark() const;

    std::string objectTypeString() const;
    std::string domainOfType(int domainType) const;

    Object *parent() const;
    virtual void setParent(Object *parent);
    virtual int childCount() const;
    virtual void resetData();
    virtual void clearData();

    virtual Object *clone() const;
    Object &operator =(const Object &other);

    // Serializable interface
public:
    virtual Json::Value save() const override;
    virtual bool restore(const Json::Value &json, int /*deep*/ = -1) override;

protected:
    Object(const Object &other);

private:
    ObjectData *d;
};

} // end of namespace Icd

#endif // ICD_OBJECT_H
