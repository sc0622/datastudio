#ifndef ICD_ITEM_ARRAY_H
#define ICD_ITEM_ARRAY_H

#include "icd_item.h"

namespace Icd {

//
enum ArrayType {
    ArrayInvalid = -1,
    ArrayI8,
    ArrayU8,
    ArrayI16,
    ArrayU16,
    ArrayI32,
    ArrayU32,
    ArrayI64,
    ArrayU64,
    ArrayF32,
    ArrayF64,
    ArrayTotal
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

    int count() const;
    void setCount(int count);

    std::string typeName() const override;
    std::string typeString() const override;
    int typeSize() const;

    Object *clone() const override;
    ArrayItem &operator =(const ArrayItem &other);

    std::string toString() const;
    int8_t *i8() const;
    uint8_t *u8() const;
    int16_t *i16() const;
    uint16_t *u16() const;
    int32_t *i32() const;
    uint32_t *u32() const;
    int64_t *i64() const;
    uint64_t *u64() const;
    float_t *f32() const;
    double_t *f64() const;

    void setData(const char *buffer, int size);

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
