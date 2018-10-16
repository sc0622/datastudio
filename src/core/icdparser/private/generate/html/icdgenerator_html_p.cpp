#include "precomp.h"
#include "icdgenerator_html_p.h"

namespace Icd {

HtmlGeneratorData::HtmlGeneratorData()
{

}

bool HtmlGeneratorData::generateTitle(const TablePtr &table, std::ostream &tableStream,
                                      const std::string &filePath)
{
    (void)(table);
    (void)(tableStream);
    (void)(filePath);

    return true;
}

bool HtmlGeneratorData::generateTypes(std::ostream &tableStream)
{
    tableStream << "<p></p>"
                   "<h2 align='center'>"
                << "Base data type definitions"
                << "</h2>";
    tableStream << "<p></p>"
                << "<table class='gridtable' align='center' border='1' summary='Desc'"
                   "cellspacing='0' cellpadding='0' style='border-collapse:collapse;border:0'>"
                   "<tr>"
                   "<th>h1</td>"
                   "<th>h2</td>"
                   "<th>h3</td>"
                   "<th>h4</td>"
                   "</tr>"
                   "<tr>"
                   "<th>Cell(0, 0)</td>"
                   "<td>Cell(0, 1)</td>"
                   "<td>Cell(0, 2)</td>"
                   "<td>Cell(0, 3)</td>"
                   "<tr>"
                   "<tr>"
                   "<th>Cell(1, 0)</td>"
                   "<td>Cell(1, 1)</td>"
                   "<td>Cell(1, 2)</td>"
                   "<td>Cell(1, 3)</td>"
                   "<tr>"
                   "<tr>"
                   "<th>Cell(2, 0)</td>"
                   "<td>Cell(2, 1)</td>"
                   "<td>Cell(2, 2)</td>"
                   "<td>Cell(2, 3)</td>"
                   "<tr>"
                   "</table>";

    return true;
}

bool HtmlGeneratorData::generatePrefix(const TablePtr &table, std::ostream &tableStream)
{
    tableStream << "<!doctype html>"
                   "<html>"
                   "<meta charset='GB2312' />"
                   "<style>";
#ifndef J_NO_QT
    QFile fileCss(":/icdparser/css/generate_html.css");
    if (fileCss.open(QFile::ReadOnly | QFile::Text)) {
        tableStream << fileCss.readAll();
    }
#endif
    tableStream << "</style>"
                   "<head>"
                << "<title>"
                << table->name()
                << "</title>"
                << "</head>"
                   "<body>";

    return true;
}

bool HtmlGeneratorData::generateSuffix(const TablePtr &table, std::ostream &tableStream)
{
    (void)(table);
    tableStream << "</body>"
                   "</html>";

    return true;
}

bool HtmlGeneratorData::generateTable(const TablePtr &table,
                                      std::ostream &tableStream)
{
    std::ostringstream memberStream;
    const Icd::ItemPtrArray &items = table->allItem();
    Icd::ItemPtrArray::const_iterator citer = items.cbegin();
    for (; citer != items.cend(); ++citer) {
        const Icd::ItemPtr &item = *citer;
        if (!generateItem(item, tableStream, memberStream)) {
            return false;
        }
    }

    return true;
}

bool HtmlGeneratorData::generateItem(const Icd::ItemPtr &item,
                                         std::ostream &tableStream,
                                         std::ostream &itemsStream)
{
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
    default:
        break;
    }

    return true;
}

bool HtmlGeneratorData::generateHeaderItem(const HeaderItemPtr &headItem,
                                           std::ostream &tableStream,
                                           std::ostream &itemsStream)
{
    (void)(headItem);
    (void)(tableStream);
    (void)(itemsStream);
    return true;
}

bool HtmlGeneratorData::generateCounterItem(const CounterItemPtr &counterItem,
                                            std::ostream &tableStream,
                                            std::ostream &itemsStream)
{
    (void)(counterItem);
    (void)(tableStream);
    (void)(itemsStream);
    return true;
}

bool HtmlGeneratorData::generateCheckItem(const CheckItemPtr &checkItem,
                                          std::ostream &tableStream,
                                          std::ostream &itemsStream)
{
    (void)(checkItem);
    (void)(tableStream);
    (void)(itemsStream);
    return true;
}

bool HtmlGeneratorData::generateFrameCodeItem(const FrameCodeItemPtr &frameCodeItem,
                                              std::ostream &tableStream,
                                              std::ostream &itemsStream)
{
    (void)(frameCodeItem);
    (void)(tableStream);
    (void)(itemsStream);
    return true;
}

bool HtmlGeneratorData::generateNumericItem(const NumericItemPtr &numericItem,
                                            std::ostream &tableStream,
                                            std::ostream &itemsStream)
{
    (void)(numericItem);
    (void)(tableStream);
    (void)(itemsStream);
    return true;
}

bool HtmlGeneratorData::generateBitItem(const BitItemPtr &bitItem,
                                        std::ostream &tableStream,
                                        std::ostream &itemsStream)
{
    (void)(bitItem);
    (void)(tableStream);
    (void)(itemsStream);
    return true;
}

bool HtmlGeneratorData::generateComplexItem(const ComplexItemPtr &complexItem,
                                            std::ostream &tableStream,
                                            std::ostream &itemsStream)
{
    (void)(tableStream);
    (void)(itemsStream);
    const Icd::TablePtr table = complexItem->table();
    if (!table) {
        return true;    // FIXME
    }

    if (!generateTable(table, tableStream)) {
        return false;
    }

    return true;
}

bool HtmlGeneratorData::generateFrameItem(const FrameItemPtr &frameItem,
                                          std::ostream &tableStream,
                                          std::ostream &itemsStream)
{
    (void)(itemsStream);

    std::ostringstream memberStream;
    (void)(memberStream);
    const Icd::TablePtrMap &tables = frameItem->allTable();
    Icd::TablePtrMap::const_iterator citer = tables.cbegin();
    for (; citer != tables.cend(); ++citer) {
        const Icd::TablePtr &table = citer->second;
        if (!generateTable(table, tableStream)) {
            return false;
        }
    }

    return true;
}

} // end of namespace Icd
