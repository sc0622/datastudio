#ifndef ICD_ITEM_ARRAY_H
#define ICD_ITEM_ARRAY_H

#include "icd_item.h"

namespace Icd {

//
enum ArrayType {
    InvalidArray = -1,
    Int8Array,
    UInt8Array,
    Int16Array,
    UInt16Array,
    Int32Array,
    UInt32Array,
    Int64Array,
    UInt64Array,
    Float32Array,
    Float64Array,
    ArrayTypeTotal
};

class ArrayItem;
class ArrayItemData;
typedef std::shared_ptr<ArrayItem> ArrayItemPtr;

class ICDCORE_EXPORT ArrayItem : public Item
{
public:
    explicit ArrayItem(Object *parent = nullptr);
    explicit ArrayItem(const std::string &id, Object *parent = nullptr);
    ~ArrayItem();

    ArrayType arrayType() const;
    void setArrayType(ArrayType type);
    std::string arrayTypeString() const;
    static std::string arrayTypeString(ArrayType type);
    static ArrayType stringArrayType(const std::string &str);

    std::string typeName() const override;
    std::string typeString() const override;

    Object *clone() const override;
    ArrayItem &operator =(const ArrayItem &other);

    // Serializable interface
public:
    Json::Value save() const override;
    bool restore(const Json::Value &json, int deep = -1) override;

protected:
    ArrayItem(const ArrayItem &other);

private:
    ArrayItemData *d;
};

}

#endif // ICD_ITEM_ARRAY_H
