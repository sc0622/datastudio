#ifndef ICD_OBJECT_H
#define ICD_OBJECT_H

#include "icdcore_global.h"
#include <string>

namespace Icd {

/**
 * @brief The ObjectType enum
 */
enum ObjectType {
    ObjectInvalid = -1,
    ObjectRoot,
    ObjectVehicle,
    ObjectSystem,
    ObjectTable,
    ObjectItem
};

/**
 * @brief The DomainType enum
 */
enum DomainType {
    DomainValid = -1,
    DomainId,
    DomainName,
    DomainMark
};

class Object;
class ObjectData;
typedef JHandlePtr<Object> ObjectPtr;

/**
 * @brief The Object class
 */
class ICDCORE_EXPORT Object : public Serializable
{
public:
    /**
     * @brief Object
     * @param type
     * @param parent
     */
    explicit Object(ObjectType type = ObjectInvalid, Object *parent = 0);

    /**
     * @brief Object
     * @param id
     * @param type
     * @param parent
     */
    explicit Object(const std::string &id, ObjectType type = ObjectInvalid,
                    Object *parent = 0);

    virtual ~Object();

    /**
     * @brief objectType
     * @return
     */
    ObjectType objectType() const;

    /**
     * @brief ��ȡID��
     * @return ID
     */
    std::string id() const;

    /**
     * @brief domain
     * @return
     */
    std::string domain() const;

    /**
     * @brief setDomain
     * @param domain
     */
    void setDomain(const std::string &domain);

    /**
     * @brief ����ID��
     * @param [in] id :��������
     */
    void setId(const std::string &id);

    /**
     * @brief ��ȡ����������
     * @return ����������
     */
    std::string name() const;

    /**
     * @brief ��������������
     * @param name : ����������
     */
    void setName(const std::string &name);

    /**
     * @brief ��ȡ�������ʶ
     * @return �������ʶ
     */
    std::string mark() const;

    /**
     * @brief ������Ŀ��ʶ
     * @param [in] mark : ��ʶ
     */
    void setMark(const std::string &mark);

    /**
     * @brief ��ȡ������������Ϣ
     * @return
     */
    std::string desc() const;

    /**
     * @brief ����������������Ϣ
     * @param [in] desc : ������Ϣ
     */
    void setDesc(const std::string &desc);

    /**
     * @brief isPrivateMark
     * @return
     */
    virtual bool isPrivateMark() const;

    /**
     * @brief parent
     * @return
     */
    Object *parent() const;

    /**
     * @brief setParent
     * @param parent
     */
    virtual void setParent(Object *parent);

    /**
     * @brief childCount
     * @return
     */
    virtual int childCount() const;

    /**
     * @brief objectTypeString
     * @return
     */
    std::string objectTypeString() const;

    /**
     * @brief resetData
     */
    virtual void resetData();

    /**
     * @brief clearData
     */
    virtual void clearData();

    /**
     * @brief clone
     * @return
     */
    virtual Object *clone() const;

    /**
     * @brief operator =
     * @param other
     * @return
     */
    Object &operator =(const Object &other);

    /**
     * @brief domainOfType
     * @param domainType
     * @return
     */
    std::string domainOfType(int domainType) const;

    // Serializable interface
public:
    virtual Json::Value save() const;
    virtual bool restore(const Json::Value &json, int /*deep*/ = -1);

protected:
    /**
     * @brief Object
     * @param other
     */
    Object(const Object &other);

private:
    ObjectData *d;
};

} // end of namespace Icd

#endif // ICD_OBJECT_H
