#ifndef ICDPARSER_SQL_P_H
#define ICDPARSER_SQL_P_H

#include "icdparser/icdparser_global.h"
#include "../icdparser_sql.h"
#include "DBAccess/dbaccess.h"
#include "KernelClass/globalstruct.h"
#include "icdcore/icdcore_inc.h"

namespace Icd {

// class SqlParserData

typedef std::shared_ptr<DBAccess> DBAccessPtr;

class SqlParserData
{
    friend class SqlParser;
public:
    SqlParserData()
        : db(DBAccessPtr(new DBAccess()))
    {

    }

    // for query
    bool queryItem(const std::string &tableId,
                   const std::string &itemId, stTableRules &rules);
    stICDBase icdBase(const std::string &id) const;
    std::vector<stICDBase> icdBaseArray(const std::string &planeID,
                                        const std::string &systemID,
                                        bool topOnly = false) const;
    stSystem systemBase(const std::string &planeID,
                        const std::string &systemID) const;
    std::vector<stSystem> systemBaseArray(const std::string &planeID) const;
    stPlane planeBase(const std::string &id) const;
    std::vector<stPlane> planeBaseArray() const;
    DMSpace::_vectorIcdTR tableRules(const std::string &planeID,
                                     const std::string &systemID) const;
    DMSpace::_vectorIcdTR tableRules(const std::string &tableId) const;
    DMSpace::pairIcdTR singleIcdTR(const DMSpace::_vectorIcdTR &rules,
                                   const stICDBase &icdBase) const;
    DMSpace::pairPS singlePS(const DMSpace::_vectorPS &ps,
                             const stPlane &plane) const;

    // for parse
    bool parseObject(const stTableRules &rule,
                     const Icd::ObjectPtr &object) const;
    bool parseItem(const stTableRules &rule, Icd::ItemPtr &item,
                   const DMSpace::_vectorIcdTR &tableRules, int deep) const;
    bool parseItemHead(const stTableRules &rule,
                       const Icd::HeaderItemPtr &head) const;
    bool parseItemCounter(const stTableRules &rule,
                          const Icd::CounterItemPtr &counter) const;
    bool parseItemCheck(const stTableRules &rule,
                        const Icd::CheckItemPtr &check) const;
    bool parseItemFrameCode(const stTableRules &rule,
                            const Icd::FrameCodeItemPtr &frameCode) const;
    bool parseItemNumeric(const stTableRules &rule,
                          const Icd::NumericItemPtr &numeric) const;
    bool parseItemBit(const stTableRules &rule,
                      const Icd::BitItemPtr &bit) const;
    bool parseItemComplex(const stTableRules &rule,
                          const Icd::ComplexItemPtr &complex,
                          const DMSpace::_vectorIcdTR &tableRules, int deep) const;
    bool parseItemFrame(const stTableRules &rule,
                        const Icd::FrameItemPtr &frame,
                        const DMSpace::_vectorIcdTR &tableRules, int deep) const;
    bool parseTable(const std::string &tableId,
                    const Icd::TablePtr &table,
                    const DMSpace::_vectorIcdTR &tableRules, int deep) const;
    bool parseSystem(const std::string &planeID,
                     const stSystem &systemBase,
                     const Icd::SystemPtr &system,
                     const DMSpace::_vectorIcdTR &tableRules, int deep) const;
    bool parseVehicle(const std::string &planeID,
                      const Icd::VehiclePtr &vehicle,
                      const DMSpace::_vectorPS &plane_system,
                      const DMSpace::_vectorIcdTR &tableRules, int deep) const;
    bool parseVehicle(Icd::VehiclePtrArray &vehicles, int deep);

    // for convert
    bool object2rule(const Icd::ObjectPtr &object, stTableRules &rule) const;
    bool Item2rule(const Icd::ItemPtr &item, stTableRules &rule) const;
    bool Item2TableRules(const Icd::ItemPtr &item, const std::string &group,
                         const std::string &parent,
                         DMSpace::_vectorIcdTR &rules) const;
    bool ItemHead2rule(const Icd::HeaderItemPtr &head, stTableRules &rule) const;
    bool ItemCounter2rule(const CounterItemPtr &counter,
                          stTableRules &rule) const;
    bool ItemCheck2rule(const CheckItemPtr &check,
                        stTableRules &rule) const;
    bool ItemFrameCode2rule(const FrameCodeItemPtr &frameCode,
                            stTableRules &rule) const;
    bool ItemFrameCode2TableRules(const FrameCodeItemPtr &frameCode,
                                  const std::string &group,
                                  const std::string &parent,
                                  DMSpace::_vectorIcdTR &rules) const;
    bool ItemNumeric2rule(const Icd::NumericItemPtr &numeric,
                          stTableRules &rule) const;
    bool ItemBit2rule(const Icd::BitItemPtr &bit, stTableRules &rule) const;
    bool ItemComplex2rule(const Icd::ComplexItemPtr &complex,
                          stTableRules &rule) const;
    bool ItemComplex2TableRules(const Icd::ComplexItemPtr &complex,
                                const std::string &group,
                                const std::string &parent,
                                DMSpace::_vectorIcdTR &rules) const;
    bool ItemFrame2rule(const Icd::FrameItemPtr &frame,
                        stTableRules &rule) const;
    bool ItemFrame2TableRules(const Icd::FrameItemPtr &frame,
                              const std::string &group,
                              const std::string &parent,
                              DMSpace::_vectorIcdTR &rules) const;
    bool table2TableRules(const Icd::TablePtr &table,
                          const std::string &group,
                          const std::string &parent,
                          DMSpace::_vectorIcdTR &rules) const;
    bool system2Base(const Icd::SystemPtr &system, stSystem &base) const;
    bool vehicle2Base(const Icd::VehiclePtr &vehicle, stPlane &base) const;

    // for save
    bool saveData(const DMSpace::_vectorPS &infrastructure,
                  const DMSpace::_vectorIcdTR &tableRules) const;

private:
    DBAccessPtr db;
};

} // end of namespace Icd

#endif // ICDPARSER_SQL_P_H
