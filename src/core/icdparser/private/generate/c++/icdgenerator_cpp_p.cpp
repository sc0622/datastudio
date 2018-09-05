#include "precomp.h"
#include "icdgenerator_cpp_p.h"
#include "../../../icdparser.h"

namespace Icd {

CppGeneratorData::CppGeneratorData(CppGenerator *q)
    : J_QPTR(q)
{

}

CppGeneratorData::~CppGeneratorData()
{

}

bool CppGeneratorData::generateHeader(QTextStream &stream)
{
    QFile *file = qobject_cast<QFile *>(stream.device());
    if (!file) {
        return false;
    }

    QFileInfo fileInfo(*file);
    QString header = headerName(fileInfo.fileName());
    stream << "#ifndef " << header << endl
           << "#define " << header << endl;

    return true;
}

bool CppGeneratorData::generateTypes(QTextStream &stream)
{
    stream << endl
           << "/* basic types definition */" << endl
           << "#ifndef DEF_ICD_TYPE" << endl
           << "#define DEF_ICD_TYPE" << endl
           << "typedef char icd_int8;" << endl
           << "typedef unsigned char icd_uint8;" << endl
           << "typedef short icd_int16;" << endl
           << "typedef unsigned short icd_uint16;" << endl
           << "typedef int icd_int32;" << endl
           << "typedef unsigned int icd_uint32;" << endl
          #ifdef _MSC_VER
           << "typedef __int64 icd_int64;" << endl
           << "typedef unsigned __int64 icd_uint64;" << endl
          #else
              //
          #endif
           << "typedef float icd_float;" << endl
           << "typedef double icd_double;" << endl
           << "typedef float icd_float32;" << endl
           << "typedef double icd_float64;" << endl
           << "#endif // DEF_ICD_TYPE" << endl;

    return true;
}

bool CppGeneratorData::generateFooter(QTextStream &stream)
{
    QFile *file = qobject_cast<QFile *>(stream.device());
    if (!file) {
        return false;
    }

    QFileInfo fileInfo(*file);
    QString header = headerName(fileInfo.fileName());
    stream << endl
           << "#endif // " << header << endl;

    return true;
}

bool CppGeneratorData::generateNamespacePrefix(QTextStream &stream)
{
    stream << endl
           << "#ifdef __cplusplus" << endl
           << "namespace Icd {" << endl
           << "#endif" << endl
           << endl
           << "#pragma pack(push)" << endl
           << "#pragma pack(1)" << endl;

    return true;
}

bool CppGeneratorData::generateNamespaceSuffix(QTextStream &stream)
{
    stream << endl
           << "#pragma pack(pop)" << endl
           << endl
           << "#ifdef __cplusplus" << endl
           << "} // end of namespace Icd" << endl
           << "#endif" << endl;

    return true;
}

bool CppGeneratorData::generateTable(const TablePtr &table,
                                     QTextStream &tableStream)
{
    if (J_QPTR->parser()->canceledSaveAs()) {
        return false;
    }

    //
    J_QPTR->parser()->setMessage(QObject::tr("Generate table document\nTable: %1")
                                 .arg(QString::fromStdString(table->name()))
                                 .toStdString());

    QString member;
    QTextStream memberStream(&member);
    const Icd::ItemPtrArray &items = table->allItem();
    Icd::ItemPtrArray::const_iterator citer = items.cbegin();
    for (; citer != items.cend(); ++citer) {
        const Icd::ItemPtr &item = *citer;
        if (!generateDataItem(item, tableStream, memberStream)) {
            return false;
        }
    }

    // format -> table (mark, name): desc
    int tableSize = 0;
    Icd::Item *parentItem = dynamic_cast<Icd::Item *>(table->parent());
    if (parentItem) {
        tableSize = static_cast<int>(std::ceil(parentItem->bufferSize()));
        int unusedSize = tableSize - static_cast<int>(std::ceil(table->bufferSize()));
        if (unusedSize > 0) {
            QString line;
            QTextStream lineStream(&line);
            lineStream << "icd_uint8 unused[" << unusedSize << "];";
            memberStream << "    ";
            memberStream.setFieldWidth(30);
            memberStream << left << line;
            memberStream.reset();
            if (line.size() >= 30) {
                memberStream << "  ";
            }
            memberStream << "/* unused */" << endl;
        }
    } else {
        tableSize = static_cast<int>(std::ceil(table->bufferSize()));
    }

    const QString tableName = QString::fromStdString(table->typeName());

    tableStream << endl << "/* " << tableName << " (size: " << tableSize << " byte(s), "
                << QString::fromStdString(table->name()) << ")";
    if (!table->desc().empty()) {
        tableStream << ": " << QString::fromStdString(table->desc());
    }
    tableStream << " */" << endl << "typedef struct _" << tableName << endl << "{" << endl;
    if (member.isEmpty()) {
        tableStream << "    icd_uint8 unused;" << endl;
    } else {
        tableStream << member;
    }
    tableStream << "} " << tableName << ";" << endl;

    return true;
}

bool CppGeneratorData::generateDataItem(const ItemPtr &item,
                                        QTextStream &tableStream,
                                        QTextStream &itemsStream)
{
    if (J_QPTR->parser()->canceledSaveAs()) {
        return false;
    }

    switch (item->type()) {
    case Icd::ItemHead:
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
        return false;
    }

    if (J_QPTR->parser()->canceledSaveAs()) {
        return false;
    }

    return true;
}

bool CppGeneratorData::generateHeaderItem(const Icd::HeaderItemPtr &headItem,
                                          QTextStream &tableStream,
                                          QTextStream &itemsStream)
{
    Q_UNUSED(tableStream);
    // format -> [    type id;    /* name (desc) */]
    QString line;
    QTextStream lineStream(&line);
    lineStream << QString::fromStdString(headItem->typeName())
               << " " << QString::fromStdString(headItem->codeName()) << ";";
    itemsStream << "    ";
    itemsStream.setFieldWidth(30);
    itemsStream << left << line;
    itemsStream.reset();
    if (line.size() >= 30) {
        itemsStream << "  ";
    }
    itemsStream << "/* " << QString::fromStdString(headItem->name());
    if (!headItem->desc().empty()) {
        itemsStream << " (" << QString::fromStdString(headItem->desc()) << ")";
    }
    itemsStream << " */" << endl;

    return true;
}

bool CppGeneratorData::generateCounterItem(const Icd::CounterItemPtr &counterItem,
                                           QTextStream &tableStream,
                                           QTextStream &itemsStream)
{
    Q_UNUSED(tableStream);
    // format -> [    type id;    /* name (desc) */]
    QString line;
    QTextStream lineStream(&line);
    lineStream << QString::fromStdString(counterItem->typeName())
               << " " << QString::fromStdString(counterItem->codeName()) << ";";
    itemsStream << "    ";
    itemsStream.setFieldWidth(30);
    itemsStream << left << line;
    itemsStream.reset();
    if (line.size() >= 30) {
        itemsStream << "  ";
    }
    itemsStream << "/* " << QString::fromStdString(counterItem->name());
    if (!counterItem->desc().empty()) {
        itemsStream << " (" << QString::fromStdString(counterItem->desc()) << ")";
    }
    itemsStream << " */" << endl;

    return true;
}

bool CppGeneratorData::generateCheckItem(const Icd::CheckItemPtr &checkItem,
                                         QTextStream &tableStream,
                                         QTextStream &itemsStream)
{
    Q_UNUSED(tableStream);
    // format -> [    type id;    /* name (desc) */]
    QString line;
    QTextStream lineStream(&line);
    lineStream << QString::fromStdString(checkItem->typeName())
               << " " << QString::fromStdString(checkItem->codeName()) << ";";
    itemsStream << "    ";
    itemsStream.setFieldWidth(30);
    itemsStream << left << line;
    itemsStream.reset();
    if (line.size() >= 30) {
        itemsStream << "  ";
    }
    itemsStream << "/* " << QString::fromStdString(checkItem->name());
    if (!checkItem->desc().empty()) {
        itemsStream << " (" << QString::fromStdString(checkItem->desc()) << ")";
    }
    itemsStream << " */" << endl;

    return true;
}

bool CppGeneratorData::generateFrameCodeItem(const Icd::FrameCodeItemPtr &frameCodeItem,
                                             QTextStream &tableStream,
                                             QTextStream &itemsStream)
{
    Q_UNUSED(tableStream);
    // format -> [    type id;    /* name (desc) */]
    QString line;
    QTextStream lineStream(&line);
    lineStream << QString::fromStdString(frameCodeItem->typeName())
               << " " << QString::fromStdString(frameCodeItem->codeName()) << ";";
    itemsStream << "    ";
    itemsStream.setFieldWidth(30);
    itemsStream << left << line;
    itemsStream.reset();
    if (line.size() >= 30) {
        itemsStream << "  ";
    }
    itemsStream << "/* " << QString::fromStdString(frameCodeItem->name());
    if (!frameCodeItem->desc().empty()) {
        itemsStream << " (" << QString::fromStdString(frameCodeItem->desc()) << ")";
    }
    itemsStream << " */" << endl;

    return true;
}

bool CppGeneratorData::generateNumericItem(const Icd::NumericItemPtr &numericItem,
                                           QTextStream &tableStream,
                                           QTextStream &itemsStream)
{
    Q_UNUSED(tableStream);
    // format -> [    type id;    /* name [scale:x, offset:x, range:(), unit:x](desc) */]
    QString line;
    QTextStream lineStream(&line);
    lineStream << QString::fromStdString(numericItem->typeName())
               << " " << QString::fromStdString(numericItem->codeName()) << ";";
    itemsStream << "    ";
    itemsStream.setFieldWidth(30);
    itemsStream << left << line;
    itemsStream.reset();
    if (line.size() >= 30) {
        itemsStream << "  ";
    }
    itemsStream << "/* "
                << QString::fromStdString(numericItem->name()) << " [scale:"
                << QString::number(numericItem->scale()) << ", offset:"
                << QString::number(numericItem->offset()) << ", range:"
                << QString::fromStdString(numericItem->limit()->toString()) << ", unix:"
                << QString::fromStdString(numericItem->unit()) << "]";
    if (!numericItem->desc().empty()) {
        itemsStream << "(" << QString::fromStdString(numericItem->desc()) << ")";
    }
    itemsStream << " */" << endl;

    return true;
}

bool CppGeneratorData::generateBitItem(const Icd::BitItemPtr &bitItem,
                                       QTextStream &tableStream,
                                       QTextStream &itemsStream)
{
    Q_UNUSED(tableStream);
    // format -> [    type id;    /* name [start:x, count:x](desc) */]
    QString line;
    QTextStream lineStream(&line);
    lineStream << QString::fromStdString(bitItem->typeName())
               << " " << QString::fromStdString(bitItem->codeName())
               << " : " << bitItem->bitCount() << ";";
    itemsStream << "    ";
    itemsStream.setFieldWidth(30);
    itemsStream << left << line;
    itemsStream.reset();
    if (line.size() >= 30) {
        itemsStream << "  ";
    }
    itemsStream << "/* "
                << QString::fromStdString(bitItem->name()) << " [start:"
                << QString::number(bitItem->bitStart()) << ", count:"
                << QString::number(bitItem->bitCount()) << "]";
    if (!bitItem->desc().empty()) {
        itemsStream << "(" << QString::fromStdString(bitItem->desc()) << ")";
    }
    itemsStream << " */" << endl;

    return true;
}

bool CppGeneratorData::generateComplexItem(const Icd::ComplexItemPtr &complexItem,
                                           QTextStream &stream,
                                           QTextStream &tableStream)
{
    const Icd::TablePtr table = complexItem->table();
    if (!table) {
        return true;    // FIXME
    }

    if (!generateTable(table, stream)) {
        return false;
    }

    // format -> [    type id;    /* name (desc) */]
    QString line;
    QTextStream lineStream(&line);
    lineStream << QString::fromStdString(complexItem->typeName())
               << " " << QString::fromStdString(complexItem->codeName()) << ";";
    tableStream << "    ";
    tableStream.setFieldWidth(30);
    tableStream << left << line;
    tableStream.reset();
    if (line.size() >= 30) {
        tableStream << "  ";
    }
    tableStream << "/* " << QString::fromStdString(complexItem->name());
    if (!complexItem->desc().empty()) {
        tableStream << " (" << QString::fromStdString(complexItem->desc()) << ")";
    }
    tableStream << " */" << endl;

    return true;
}

bool CppGeneratorData::generateFrameItem(const Icd::FrameItemPtr &frameItem,
                                         QTextStream &tableStream,
                                         QTextStream &itemsStream)
{
    if (!frameItem) {
        return false;
    }

    QString member;
    QTextStream memberStream(&member);
    const Icd::TablePtrMap &tables = frameItem->allTable();
    Icd::TablePtrMap::const_iterator citer = tables.cbegin();
    for (; citer != tables.cend(); ++citer) {
        const Icd::TablePtr &table = citer->second;
        if (!generateTable(table, tableStream)) {
            return false;
        }
        // format -> [    type id;    /* name (desc) */]
        QString line;
        QTextStream lineStream(&line);
        lineStream << "        "
                   << QString::fromStdString(table->typeName()) << " "
                   << QString::fromStdString(table->codeName()) << ";";
        memberStream.setFieldWidth(50);
        memberStream << left << line;
        memberStream.reset();
        if (line.size() >= 50) {
            memberStream << "  ";
        }
        memberStream << "/* " << QString::fromStdString(table->name());
        if (!table->desc().empty()) {
            memberStream << " (" << QString::fromStdString(table->desc()) << ")";
        }
        memberStream << " */" << endl;
    }

    // format -> [    type id;    /* name (desc) */]
    QString line;
    QTextStream lineStream(&line);
    lineStream << "} " << QString::fromStdString(frameItem->codeName()) << ";";
    itemsStream << "    union " << QString::fromStdString(frameItem->typeName())
                << " {" << endl << member << "    ";
    itemsStream.setFieldWidth(25);
    itemsStream << left << line;
    itemsStream.reset();
    if (line.size() >= 30) {
        itemsStream << "  ";
    }
    itemsStream << "/* " << QString::fromStdString(frameItem->name());
    if (!frameItem->desc().empty()) {
        itemsStream << " (" << QString::fromStdString(frameItem->desc()) << ")";
    }
    itemsStream << " */" << endl;

    return true;
}

bool CppGeneratorData::generateDateTimeItem(const DateTimeItemPtr &dateTimeItem,
                                            QTextStream &tableStream, QTextStream &itemsStream)
{
    Q_UNUSED(dateTimeItem);
    Q_UNUSED(tableStream);
    Q_UNUSED(itemsStream);
    Q_ASSERT(false);        //TODO [not supported]
    return false;
}

bool CppGeneratorData::generateArrayItem(const ArrayItemPtr &arrayItem,
                                         QTextStream &tableStream, QTextStream &itemsStream)
{
    Q_UNUSED(tableStream);
    if (!arrayItem) {
        return false;
    }

    // format -> [    type id[size];    /* name (desc) */]
    QString line;
    QTextStream lineStream(&line);
    lineStream << QString::fromStdString(arrayItem->typeName())
               << " " << QString::fromStdString(arrayItem->codeName())
               << "[" << arrayItem->count() << "];";
    itemsStream << "    ";
    itemsStream.setFieldWidth(30);
    itemsStream << left << line;
    itemsStream.reset();
    if (line.size() >= 30) {
        itemsStream << "  ";
    }
    itemsStream << "/* " << QString::fromStdString(arrayItem->name());
    if (!arrayItem->desc().empty()) {
        itemsStream << " (" << QString::fromStdString(arrayItem->desc()) << ")";
    }
    itemsStream << " */" << endl;

    return true;
}

QString CppGeneratorData::headerName(const QString &fileName)
{
    QString name = QString(fileName).replace(QRegExp("[^0-9a-zA-z]"), "_");
    if (name.count() > 0 && name.at(0).isDigit()) {
        name.prepend('_');
    }
    return name.toUpper();
}

} // end of namespace Icd
