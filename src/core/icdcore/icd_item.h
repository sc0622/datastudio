#ifndef ICD_ITEM_H
#define ICD_ITEM_H

#include "icd_object.h"
#include <vector>

namespace Icd {

enum ItemType {
    ItemInvalid = -1,
    ItemHeader,
    ItemCounter,
    ItemCheck,
    ItemFrameCode,
    ItemNumeric,
    ItemArray,
    ItemBitMap,
    ItemBitValue,
    ItemDateTime,
    ItemComplex,
    ItemFrame,
    ItemTotal
};

class Table;

class Item;
class ItemData;
typedef std::shared_ptr<Item> ItemPtr;
typedef std::vector<ItemPtr> ItemPtrArray;

class ICDCORE_EXPORT Item : public Object
{
public:
    explicit Item(ItemType type = ItemInvalid, Object *parent = nullptr);
    explicit Item(const std::string &id, ItemType type = ItemInvalid,
                  Object *parent = nullptr);
    Item(const Item &other);
    virtual ~Item() override;

    virtual int rtti() const override;
    virtual bool isSimpleItem() const override;

    ItemType type() const;

    int itemOffset() const;
    char *buffer() const;
    virtual double bufferSize() const;
    virtual double bufferOffset() const;
    virtual double localOffset() const;

    virtual double data() const { return 0.0; }
    virtual void setData(double d) { (void)d; }
    virtual std::string dataString() const { return ""; }
    double defaultValue() const;
    void setDefaultValue(double value);

    virtual double dataFromBuffer(const char *buffer) const;

    virtual std::string typeName() const;
    virtual std::string codeName() const;

    virtual std::string typeString() const;
    static std::string typeString(ItemType type);
    static ItemType stringType(const std::string &str);

    virtual void resetData() override;
    virtual void clearData() override;

    virtual ObjectPtr copy() const override;
    virtual ObjectPtr clone() const override;
    Item &operator =(const Item &other);
    static ItemPtr create(const std::string &id, ItemType type, Object *parent = nullptr);
    static ItemPtr create(ItemType type, Object *parent = nullptr);

    bool isSubFrameItem() const;
    Icd::Table *subFrameTable() const;

    static bool fuzzyCompare(double p1, double p2);
    static bool fuzzyCompare(float p1, float p2);

protected:
    void setType(ItemType type);

    virtual void setBuffer(char *buffer);
    virtual void setItemOffset(int offset);
    virtual void setBufferSize(double size);
    virtual void setBufferOffset(double offset);
    virtual void adjustBufferOffset();

    // Serializable interface
public:
    virtual Json::Value save() const override;
    virtual bool restore(const Json::Value &json, int deep = -1) override;

private:
    ItemData *d;
    friend class Table;
    friend class TableData;
    friend class JsonParser;
    friend class XmlParser;
    friend class SqlParser;
};

} // end of namespace Icd

#endif // ICD_ITEM_H
