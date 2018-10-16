#include "precomp.h"
#include "icdgenerator_cpp.h"
#include "icdgenerator_cpp_p.h"
#include "../../../icdparser.h"
#include "../../../../icdwidget/icdwidget_global.h"

namespace Icd {

// class CppGenerator

CppGenerator::CppGenerator(Icd::Parser *parser)
    : Generator(GeneratorTypeCpp, parser)
    , d(new CppGeneratorData(this))
{

}

CppGenerator::~CppGenerator()
{

}
#ifndef J_NO_QT
bool CppGenerator::generate(const QStandardItem *item, bool exportAll, bool rt,
                            const std::string &filePath)
{
    Q_UNUSED(exportAll);
    Q_UNUSED(rt);
    if (!item) {
        return false;
    }

    const int itemType = item->type();
    if (itemType != Icd::TreeItemTypeTable) {
        return false;
    }

    //
    parser()->setMessage(QObject::tr("Query Table\nTable: %1")
                         .arg(item->text())
                         .toStdString());

    const QString domain = item->data(Icd::TreeItemDomainRole).toString();
    Icd::TablePtr table;
    if (!parser()->parse(domain.section('/', 0, 0).toStdString(),
                         domain.section('/', 1, 1).toStdString(),
                         domain.section('/', 2).toStdString(),
                         table, Icd::ObjectItem)) {
        return false;
    }

    return generate(table, filePath);
}
#endif
bool CppGenerator::generate(const TablePtr &table, const std::string &filePath)
{
    if (!table) {
        return false;
    }
#ifndef J_NO_QT
    QFile file(QString::fromStdString(filePath));
    if (!file.open(QIODevice::Text | QIODevice::WriteOnly)) {
        return false;
    }

    if (!startup()) {
        return false;
    }

    QTextStream tableStream(&file);
    //tableStream.setCodec("GB2312");

    // generate header
    if (!d->generateHeader(tableStream)) {
        return false;
    }

    // generate types
    if (!d->generateTypes(tableStream)) {
        return false;
    }

    // generate namespace prefix
    if (!d->generateNamespacePrefix(tableStream)) {
        return false;
    }

    // generate table information
    if (!d->generateTable(table, tableStream)) {
        return false;
    }

    // generate namespace suffix
    if (!d->generateNamespaceSuffix(tableStream)) {
        return false;
    }

    // generate footer
    if (!d->generateFooter(tableStream)) {
        return false;
    }

    shutdown();

    return true;
#else
    (void)filePath;
    return false;
#endif
}

bool CppGenerator::startup()
{
    if (!Generator::startup()) {
        return false;
    }

    return true;
}

void CppGenerator::shutdown()
{
    Generator::shutdown();
}

} // end of namespace Icd
