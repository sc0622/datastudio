#include "precomp.h"
#include "icdgenerator_cpp_p.h"
#include "../../../icdparser.h"
#include <iomanip>
#include <algorithm>

namespace Icd {

CppGeneratorData::CppGeneratorData(CppGenerator *q)
    : q_ptr_(q)
{

}

CppGeneratorData::~CppGeneratorData()
{

}

bool CppGeneratorData::generateHeader(std::ostream &stream, const std::string &filePath)
{
    const std::string header = headerName(filePath);
    stream << "#ifndef " << header << std::endl
           << "#define " << header << std::endl;

    return true;
}

bool CppGeneratorData::generateTypes(std::ostream &stream)
{
    stream << std::endl
           << "/* basic types definition */" << std::endl
           << "#ifndef DEF_ICD_TYPE" << std::endl
           << "#define DEF_ICD_TYPE" << std::endl
           << "typedef char icd_int8;" << std::endl
           << "typedef unsigned char icd_uint8;" << std::endl
           << "typedef short icd_int16;" << std::endl
           << "typedef unsigned short icd_uint16;" << std::endl
           << "typedef int icd_int32;" << std::endl
           << "typedef unsigned int icd_uint32;" << std::endl
          #ifdef _MSC_VER
           << "typedef __int64 icd_int64;" << std::endl
           << "typedef unsigned __int64 icd_uint64;" << std::endl
          #else
              //
          #endif
           << "typedef float icd_float;" << std::endl
           << "typedef double icd_double;" << std::endl
           << "typedef float icd_float32;" << std::endl
           << "typedef double icd_float64;" << std::endl
           << "#endif // DEF_ICD_TYPE" << std::endl;

    return true;
}

bool CppGeneratorData::generateFooter(std::ostream &stream, const std::string &filePath)
{
    const std::string header = headerName(filePath);
    stream << std::endl
           << "#endif // " << header << std::endl;

    return true;
}

bool CppGeneratorData::generateNamespacePrefix(std::ostream &stream)
{
    stream << std::endl
           << "#ifdef __cplusplus" << std::endl
           << "namespace Icd {" << std::endl
           << "#endif" << std::endl
           << std::endl
           << "#pragma pack(push)" << std::endl
           << "#pragma pack(1)" << std::endl;

    return true;
}

bool CppGeneratorData::generateNamespaceSuffix(std::ostream &stream)
{
    stream << std::endl
           << "#pragma pack(pop)" << std::endl
           << std::endl
           << "#ifdef __cplusplus" << std::endl
           << "} // end of namespace Icd" << std::endl
           << "#endif" << std::endl;

    return true;
}

bool CppGeneratorData::generateTable(const TablePtr &table, std::ostream &tableStream)
{
    if (q_ptr_->parser()->canceledSaveAs()) {
        return false;
    }

    std::ostringstream oss;
    oss << "Generate table document\nTable: " << table->name();
    q_ptr_->parser()->setMessage(oss.str());
    oss.str("");

    std::ostringstream member;
    const Icd::ItemPtrArray &items = table->allItem();
    Icd::ItemPtrArray::const_iterator citer = items.cbegin();
    for (; citer != items.cend(); ++citer) {
        const Icd::ItemPtr &item = *citer;
        if (!generateDataItem(item, tableStream, member)) {
            return false;
        }
    }

    // format -> table (mark, name): desc
    int tableSize = 0;
    Icd::Item *parentItem = dynamic_cast<Icd::Item *>(table->parent());
    if (parentItem) {
        tableSize = parentItem->bufferSize();
        const int unusedSize = tableSize - table->bufferSize();
        if (unusedSize > 0) {
            // line
            std::ostringstream line;
            line << "icd_uint8 unused[" << unusedSize << "];";
            const std::string lineStr = line.str();
            line.str("");
            //
            member << "    ";
            const std::ios::fmtflags flags(member.flags());
            member << std::setw(30) << std::left << lineStr;
            member.setf(flags);
            if (lineStr.size() >= 30) {
                member << "  ";
            }
            member << "/* unused */" << std::endl;
        }
    } else {
        tableSize = table->bufferSize();
    }

    const std::string memberStr = member.str();
    member.str("");

    const std::string tableName = table->typeName();

    tableStream << std::endl << "/* " << tableName << " (size: " << tableSize << " byte(s), "
                << table->name() << ")";
    if (!table->desc().empty()) {
        tableStream << ": " << table->desc();
    }
    tableStream << " */" << std::endl << "typedef struct _" << tableName << std::endl
                << "{" << std::endl;
    if (memberStr.empty()) {
        tableStream << "    icd_uint8 unused;" << std::endl;
    } else {
        tableStream << memberStr;
    }
    tableStream << "} " << tableName << ";" << std::endl;

    return true;
}

bool CppGeneratorData::generateDataItem(const ItemPtr &item, std::ostream &tableStream,
                                        std::ostream &itemsStream)
{
    if (q_ptr_->parser()->canceledSaveAs()) {
        return false;
    }

    switch (item->type()) {
    case Icd::ItemHeader:
        return generateHeaderItem(JHandlePtrCast<Icd::HeaderItem>(item),
                                  tableStream, itemsStream);
    case Icd::ItemCounter:
        return generateCounterItem(JHandlePtrCast<Icd::CounterItem>(item),
                                   tableStream, itemsStream);
    case Icd::ItemCheck:
        return generateCheckItem(JHandlePtrCast<Icd::CheckItem>(item),
                                 tableStream, itemsStream);
    case Icd::ItemFrameCode:
        return generateFrameCodeItem(JHandlePtrCast<Icd::FrameCodeItem>(item),
                                     tableStream, itemsStream);
    case Icd::ItemNumeric:
        return generateNumericItem(JHandlePtrCast<Icd::NumericItem>(item),
                                   tableStream, itemsStream);
    case Icd::ItemBitMap:
    case Icd::ItemBitValue:
        return generateBitItem(JHandlePtrCast<Icd::BitItem>(item),
                               tableStream, itemsStream);
    case Icd::ItemComplex:
        return generateComplexItem(JHandlePtrCast<Icd::ComplexItem>(item),
                                   tableStream, itemsStream);
    case Icd::ItemFrame:
        return generateFrameItem(JHandlePtrCast<Icd::FrameItem>(item),
                                 tableStream, itemsStream);
    case Icd::ItemDateTime:
        return generateDateTimeItem(JHandlePtrCast<Icd::DateTimeItem>(item),
                                    tableStream, itemsStream);
    case Icd::ItemArray:
        return generateArrayItem(JHandlePtrCast<Icd::ArrayItem>(item),
                                 tableStream, itemsStream);
    default:
        break;
    }

    return true;
}

bool CppGeneratorData::generateHeaderItem(const Icd::HeaderItemPtr &headItem, std::ostream &tableStream,
                                          std::ostream &itemsStream)
{
    (void)(tableStream);
    // line - format -> [    type id;    /* name (desc) */]
    std::ostringstream line;
    line << headItem->typeName() << " " << headItem->codeName() << ";";
    const std::string lineStr = line.str();
    line.str("");
    //
    itemsStream << "    ";
    const std::ios::fmtflags flags(itemsStream.flags());
    itemsStream << std::setw(30) << std::left << lineStr;
    itemsStream.setf(flags);
    if (lineStr.size() >= 30) {
        itemsStream << "  ";
    }
    itemsStream << "/* " << headItem->name();
    if (!headItem->desc().empty()) {
        itemsStream << " (" << headItem->desc() << ")";
    }
    itemsStream << " */" << std::endl;

    return true;
}

bool CppGeneratorData::generateCounterItem(const Icd::CounterItemPtr &counterItem,
                                           std::ostream &tableStream, std::ostream &itemsStream)
{
    (void)(tableStream);
    // line - format -> [    type id;    /* name (desc) */]
    std::ostringstream line;
    line << counterItem->typeName() << " " << counterItem->codeName() << ";";
    const std::string lineStr = line.str();
    line.str("");
    //
    itemsStream << "    ";
    const std::ios::fmtflags flags(itemsStream.flags());
    itemsStream << std::setw(30) << std::left << lineStr;
    itemsStream.setf(flags);
    if (lineStr.size() >= 30) {
        itemsStream << "  ";
    }
    itemsStream << "/* " << counterItem->name();
    if (!counterItem->desc().empty()) {
        itemsStream << " (" << counterItem->desc() << ")";
    }
    itemsStream << " */" << std::endl;

    return true;
}

bool CppGeneratorData::generateCheckItem(const Icd::CheckItemPtr &checkItem,
                                         std::ostream &tableStream, std::ostream &itemsStream)
{
    (void)(tableStream);
    // line - format -> [    type id;    /* name (desc) */]
    std::ostringstream line;
    line << checkItem->typeName() << " " << checkItem->codeName() << ";";
    const std::string lineStr = line.str();
    line.str("");
    //
    itemsStream << "    ";
    const std::ios::fmtflags flags(itemsStream.flags());
    itemsStream << std::setw(30) << std::left << lineStr;
    itemsStream.setf(flags);
    if (lineStr.size() >= 30) {
        itemsStream << "  ";
    }
    itemsStream << "/* " << checkItem->name();
    if (!checkItem->desc().empty()) {
        itemsStream << " (" << checkItem->desc() << ")";
    }
    itemsStream << " */" << std::endl;

    return true;
}

bool CppGeneratorData::generateFrameCodeItem(const Icd::FrameCodeItemPtr &frameCodeItem,
                                             std::ostream &tableStream, std::ostream &itemsStream)
{
    (void)(tableStream);
    // line - format -> [    type id;    /* name (desc) */]
    std::ostringstream line;
    line << frameCodeItem->typeName() << " " << frameCodeItem->codeName() << ";";
    const std::string lineStr = line.str();
    line.str("");
    //
    itemsStream << "    ";
    const std::ios::fmtflags flags(itemsStream.flags());
    itemsStream << std::setw(30) << std::left << lineStr;
    itemsStream.setf(flags);
    if (lineStr.size() >= 30) {
        itemsStream << "  ";
    }
    itemsStream << "/* " << frameCodeItem->name();
    if (!frameCodeItem->desc().empty()) {
        itemsStream << " (" << frameCodeItem->desc() << ")";
    }
    itemsStream << " */" << std::endl;

    return true;
}

bool CppGeneratorData::generateNumericItem(const Icd::NumericItemPtr &numericItem,
                                           std::ostream &tableStream, std::ostream &itemsStream)
{
    (void)(tableStream);
    // line - format -> [    type id;    /* name [scale:x, offset:x, range:(), unit:x](desc) */]
    std::ostringstream line;
    line << numericItem->typeName() << " " << numericItem->codeName() << ";";
    const std::string lineStr = line.str();
    line.str("");
    //
    itemsStream << "    ";
    const std::ios::fmtflags flags(itemsStream.flags());
    itemsStream << std::setw(30) << std::left << lineStr;
    itemsStream.setf(flags);
    if (lineStr.size() >= 30) {
        itemsStream << "  ";
    }
    itemsStream << "/* "
                << numericItem->name() << " [scale:"
                << numericItem->scale() << ", offset:"
                << numericItem->offset() << ", range:"
                << numericItem->limit()->toString() << ", unix:"
                << numericItem->unit() << "]";
    if (!numericItem->desc().empty()) {
        itemsStream << "(" << numericItem->desc() << ")";
    }
    itemsStream << " */" << std::endl;

    return true;
}

bool CppGeneratorData::generateBitItem(const Icd::BitItemPtr &bitItem,
                                       std::ostream &tableStream, std::ostream &itemsStream)
{
    (void)(tableStream);
    // line - format -> [    type id;    /* name [start:x, count:x](desc) */]
    std::ostringstream line;
    line << bitItem->typeName() << " " << bitItem->codeName() << " : " << bitItem->bitCount() << ";";
    const std::string lineStr = line.str();
    line.str("");
    //
    itemsStream << "    ";
    const std::ios::fmtflags flags(itemsStream.flags());
    itemsStream << std::setw(30) << std::left << lineStr;
    itemsStream.setf(flags);
    if (lineStr.size() >= 30) {
        itemsStream << "  ";
    }
    itemsStream << "/* "
                << bitItem->name() << " [start:"
                << bitItem->bitStart() << ", count:"
                << bitItem->bitCount() << "]";
    if (!bitItem->desc().empty()) {
        itemsStream << "(" << bitItem->desc() << ")";
    }
    itemsStream << " */" << std::endl;

    return true;
}

bool CppGeneratorData::generateComplexItem(const Icd::ComplexItemPtr &complexItem,
                                           std::ostream &tableStream, std::ostream &itemsStream)
{
    const Icd::TablePtr table = complexItem->table();
    if (!table) {
        return true;    // FIXME
    }

    if (!generateTable(table, tableStream)) {
        return false;
    }

    // line - format -> [    type id;    /* name (desc) */]
    std::ostringstream line;
    line << complexItem->typeName() << " " << complexItem->codeName() << ";";
    const std::string lineStr = line.str();
    line.str("");
    //
    itemsStream << "    ";
    const std::ios::fmtflags flags(itemsStream.flags());
    itemsStream << std::setw(30) << std::left << lineStr;
    itemsStream.setf(flags);
    if (lineStr.size() >= 30) {
        itemsStream << "  ";
    }
    tableStream << "/* " << complexItem->name();
    if (!complexItem->desc().empty()) {
        tableStream << " (" << complexItem->desc() << ")";
    }
    tableStream << " */" << std::endl;

    return true;
}

bool CppGeneratorData::generateFrameItem(const Icd::FrameItemPtr &frameItem,
                                         std::ostream &tableStream, std::ostream &itemsStream)
{
    if (!frameItem) {
        return false;
    }

    std::ostringstream member;
    const Icd::TablePtrMap &tables = frameItem->allTable();
    Icd::TablePtrMap::const_iterator citer = tables.cbegin();
    for (; citer != tables.cend(); ++citer) {
        const Icd::TablePtr &table = citer->second;
        if (!generateTable(table, tableStream)) {
            return false;
        }
        // line - format -> [    type id;    /* name (desc) */]
        std::ostringstream line;
        line << "        " << table->typeName() << " " << table->codeName() << ";";
        const std::string lineStr = line.str();
        line.str("");
        //
        member << "    ";
        const std::ios::fmtflags flags(member.flags());
        member << std::setw(30) << std::left << lineStr;
        member.setf(flags);
        if (lineStr.size() >= 30) {
            member << "  ";
        }
        member << "/* " << table->name();
        if (!table->desc().empty()) {
            member << " (" << table->desc() << ")";
        }
        member << " */" << std::endl;
    }

    const std::string memberStr = member.str();
    member.str("");

    // line - format -> [    type id;    /* name (desc) */]
    std::ostringstream line;
    line << "} " << frameItem->codeName() << ";";
    const std::string lineStr = line.str();
    line.str("");
    //
    itemsStream << "    union " << frameItem->typeName()
                << " {" << std::endl << memberStr << "    ";
    const std::ios::fmtflags flags(itemsStream.flags());
    itemsStream << std::setw(25) << std::left << lineStr;
    itemsStream.setf(flags);
    if (lineStr.size() >= 30) {
        itemsStream << "  ";
    }
    itemsStream << "/* " << frameItem->name();
    if (!frameItem->desc().empty()) {
        itemsStream << " (" << frameItem->desc() << ")";
    }
    itemsStream << " */" << std::endl;

    return true;
}

bool CppGeneratorData::generateDateTimeItem(const DateTimeItemPtr &dateTimeItem,
                                            std::ostream &tableStream, std::ostream &itemsStream)
{
    (void)(dateTimeItem);
    (void)(tableStream);
    (void)(itemsStream);
    //TODO
    return false;
}

bool CppGeneratorData::generateArrayItem(const ArrayItemPtr &arrayItem,
                                         std::ostream &tableStream, std::ostream &itemsStream)
{
    (void)(tableStream);
    if (!arrayItem) {
        return false;
    }

    // line - format -> [    type id[size];    /* name (desc) */]
    std::ostringstream line;
    line << arrayItem->typeName() << " " << arrayItem->codeName()
         << "[" << arrayItem->count() << "];";
    const std::string lineStr = line.str();
    line.str("");
    //
    itemsStream << "    ";
    const std::ios::fmtflags flags(itemsStream.flags());
    itemsStream << std::setw(30) << std::left << lineStr;
    itemsStream.setf(flags);
    if (lineStr.size() >= 30) {
        itemsStream << "  ";
    }
    itemsStream << "/* " << arrayItem->name();
    if (!arrayItem->desc().empty()) {
        itemsStream << " (" << arrayItem->desc() << ")";
    }
    itemsStream << " */" << std::endl;

    return true;
}

std::string CppGeneratorData::headerName(const std::string &filePath)
{
    std::string::size_type index = std::max(filePath.find_last_of('/'),
                                            filePath.find_last_of('\\'));
    std::string name;
    if (index != std::string::npos) {
        name = filePath.substr(index + 1);
    } else {
        name = filePath;
    }

    for (std::string::iterator iter = name.begin();
         iter != name.end(); ++iter) {
        char ch = *iter;
        if ('a' <= ch && ch <= 'z') {
            ch += 0x20;
        }
        if ((ch < '0' || ch > '9') && (ch < 'A' || ch > 'Z')) {
            ch = '_';
        }
        *iter = ch;
    }

    if (name.size() > 0) {
        const char ch = name.at(0);
        if ('0' <= ch && ch <= '9') {
            name = '_' + name;
        }
    }

    return name;
}

} // end of namespace Icd
