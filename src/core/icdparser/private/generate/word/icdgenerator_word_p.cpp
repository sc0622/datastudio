#include "precomp.h"
#include "icdgenerator_word_p.h"
#include <QAxObject>
#include <QStandardItem>
#include "icdcore/icdcore_inc.h"
#if defined(ICDWORKER_LIB)
#include "icdworker/icdworker_inc.h"
#endif
#include "../../../../icdwidget/icdwidget_global.h"
#include "../../../icdparser.h"

namespace Icd {

WordGeneratorData::WordGeneratorData(WordGenerator *q)
    : J_QPTR(q)
    , word_(nullptr)
    , document_(nullptr)
    , tableCaption_(nullptr)
    , listTemplate_(nullptr)
    , tables_(nullptr)
    , selection_(nullptr)
{

}

WordGeneratorData::~WordGeneratorData()
{

}

bool WordGeneratorData::startup()
{
    if (word_) {
        word_->deleteLater();
    }

    //
    J_QPTR->parser()->setMessage(QStringLiteral("初始化文档基本属性").toStdString());

    //
#ifdef _MSC_VER
    HRESULT ret = ::OleInitialize(nullptr);
    if (ret != S_OK || ret == S_FALSE) {
        //return false;
    }
#endif // _MSC_VER

    word_ = new QAxObject("Word.Application");
    if (word_->isNull()) {
        return false;
    }

    //
    word_->setProperty("Visible", false);
    word_->setProperty("WindowState", 0);
    word_->setProperty("DisplatAlerts", 0);

    //
    document_ = word_->querySubObject("Documents")->querySubObject("Add()");
    if (!document_) {
        word_->deleteLater();
        return false;
    }

    //
    tables_ = document_->querySubObject("Tables");
    selection_ = word_->querySubObject("Selection");

    //
    QAxObject *captionLabels = word_->querySubObject("CaptionLabels");
    tableCaption_ = captionLabels->querySubObject("Add(QString)", QStringLiteral("表"));
    if (!tableCaption_) {
        word_->deleteLater();
        return false;
    }
    tableCaption_->setProperty("Position", 0); // above

    // list templates
    QAxObject *listGalleries = word_->querySubObject("ListGalleries");
    QAxObject *listGallery = listGalleries->querySubObject("Item(int)", 3);
    QAxObject *listTemplates = listGallery->querySubObject("ListTemplates");
    listTemplate_ = listTemplates->querySubObject("Item(int)", 5);
    QAxObject *listLevels = listTemplate_->querySubObject("ListLevels");
    const int levelCount = listLevels->property("Count").toInt();
    for (int i = 1; i <= levelCount; ++i) {
        QAxObject *listLevel = listLevels->querySubObject("Item(int)", i);
        QString numberFormat;
        for (int j = 1; j <= i; ++j) {
            numberFormat.append('%' + QString::number(j) + '.');
        }
        listLevel->setProperty("NumberFormat", numberFormat);
        const QString linkedStyle = QStringLiteral("标题 %1").arg(i);
        listLevel->setProperty("LinkedStyle", linkedStyle);
        QAxObject *font = listLevel->querySubObject("Font");
        font->setProperty("Bold", 1);
        font->setProperty("Size", 12);
        //font->setProperty("Name", QStringLiteral("宋体"));
        //
        QAxObject *styles = document_->querySubObject("Styles");
        QAxObject *style = styles->querySubObject("Item(QString)", linkedStyle);
        QAxObject *paragraphFormat = style->querySubObject("ParagraphFormat");
        paragraphFormat->setProperty("LineSpacingRule", 1);
    }

    return true;
}

void WordGeneratorData::shutdown(const QString &filePath, int saveAsType)
{
    if (!word_) {
        return;
    }

    //
    J_QPTR->parser()->setMessage(QStringLiteral("释放文档临时资源").toStdString());

    //
    if (!filePath.isEmpty()) {
        if (saveAsType == 0) {
            document_->dynamicCall("SaveAs(QString,int)",
                                  QDir::toNativeSeparators(filePath), saveAsType);
        } else {
            QVariantList params;
            params << QDir::toNativeSeparators(filePath)
                   << saveAsType << false << 0 << 0 << 1 << 1 << 0 << true << true << 1 << true;
            document_->dynamicCall("ExportAsFixedFormat(QString,int,boolean,int,int,int,"
                                  "int,int,boolean,boolean,int,boolean)",
                                  params);
        }
    }
    document_->dynamicCall("Close(boolean)", false);
    word_->dynamicCall("Quit()");

    word_->deleteLater();
    word_ = nullptr;
    document_ = nullptr;
    tableCaption_ = nullptr;

    //
#ifdef _MSC_VER
    ::OleUninitialize();
#endif // _MSC_VER
}

bool WordGeneratorData::generateDocument(const QStandardItem *item, bool exportAll, bool rt)
{
    if (!item || !document_) {
        return false;
    }

    //
    bool result = false;
    const int itemType = item->type();
    switch (itemType) {
    case TreeItemTypeRoot:
        result = generateRoot(item, exportAll, rt, 1);
        break;
    case TreeItemTypeVehicle:
        result = generateVehicle(item, exportAll, rt, 1);
        break;
    case TreeItemTypeSystem:
        result = generateSystem(item, exportAll, rt, 1);
        break;
    case TreeItemTypeTable:
    case TreeItemTypeItemTable:
        result = generateTable(item, exportAll, rt, 1);
        break;
    case TreeItemTypeDataItem:
        result = generateDataItem(item, exportAll, rt, 1);
        break;
    default:
        return false;
    }

    if (!result) {
        return false;
    }

    if (!generateContents()) {
        return false;
    }

    return true;
}

bool WordGeneratorData::generateDocument(const TablePtr &table)
{
    if (!table || !document_) {
        return false;
    }

    if (!generateTable(table, 1)) {
        return false;
    }

    if (!generateContents()) {
        return false;
    }

    return true;
}

bool WordGeneratorData::generateType()
{
    return true;
}

bool WordGeneratorData::generateRoot(const QStandardItem *itemRoot, bool exportAll,
                                     bool rt, int level)
{
    if (!itemRoot) {
        return false;
    }

    if (exportAll) {
        //
        J_QPTR->parser()->setMessage(QStringLiteral("获取所有机型数据\n%1")
                                    .arg(itemRoot->text().remove(QRegExp("<([^>]*)>")))
                                    .toStdString());

        Icd::VehiclePtrArray vehicles;
        if (!J_QPTR->parser()->parse(vehicles, Icd::ObjectVehicle)) {
            return false;
        }
        Icd::VehiclePtrArray::const_iterator citer = vehicles.cbegin();
        for (; citer != vehicles.cend(); ++citer) {
            const Icd::VehiclePtr &vehicle = *citer;
            if (!generateVehicle(vehicle, exportAll, level)) {
                return false;
            }
        }
    } else {
        int count = itemRoot->rowCount();
        for (int i = 0; i < count; ++i) {
            const QStandardItem *itemVehicle = itemRoot->child(i);
            if (!itemVehicle) {
                continue;
            }
            if (!generateVehicle(itemVehicle, exportAll, rt, level)) {
                return false;
            }
        }
    }

    return true;
}

bool WordGeneratorData::generateVehicle(const QStandardItem *itemVehicle, bool exportAll,
                                        bool rt, int level)
{
    if (!itemVehicle) {
        return false;
    }

    //
    J_QPTR->parser()->setMessage(QStringLiteral("获取机型数据\n机型：%1")
                                .arg(itemVehicle->text().remove(QRegExp("<([^>]*)>")))
                                .toStdString());

    //
    const QString vehicleId = itemVehicle->data(TreeItemDomainRole).toString();
    Icd::VehiclePtr vehicle;
    if (!J_QPTR->parser()->parse(vehicleId.toStdString(), vehicle, Icd::ObjectVehicle)) {
        return false;
    }

    if (exportAll) {
        if (!generateVehicle(vehicle, exportAll, level)) {
            return false;
        }
    } else {
        if (!generateVehicle(vehicle, false, level)) {
            return false;
        }

        //
        ++level;

        //
        int count = itemVehicle->rowCount();
        for (int i = 0; i < count; ++i) {
            const QStandardItem *itemSystem = itemVehicle->child(i);
            if (!itemSystem) {
                continue;
            }
            if (!generateSystem(itemSystem, exportAll, rt, level)) {
                return false;
            }
        }
    }

    return true;
}

bool WordGeneratorData::generateVehicle(const VehiclePtr &vehicle, bool exportAll, int level)
{
    //
    J_QPTR->parser()->setMessage(QStringLiteral("生成机型文档\n机型：%1")
                                .arg(QString::fromStdString(vehicle->name()))
                                .toStdString());

    //
    selection_->dynamicCall("EndKey(int)", 6);

    // 标题
    generateHeading(QString("%1 [%2]")
                    .arg(QString::fromStdString(vehicle->name()))
                    .arg(QString::fromStdString(vehicle->objectTypeString()).toUpper()), level);

    // 正文1
    QAxObject *paragraphs = selection_->querySubObject("Range")->querySubObject("Paragraphs");
    paragraphs->setProperty("OutlineLevel", 10);
    QAxObject *paragraphFormat = selection_->querySubObject("Range")->querySubObject("ParagraphFormat");
    paragraphFormat->setProperty("CharacterUnitFirstLineIndent", 2);
    QString vehicleDesc = QString::fromStdString(vehicle->desc());
    if (!vehicleDesc.isEmpty() && !vehicleDesc.endsWith(QStringLiteral("。"))) {
        vehicleDesc.append(QStringLiteral("。"));
    }
    selection_->dynamicCall("TypeText(QString)", vehicleDesc);
    selection_->dynamicCall("TypeParagraph()");

    //
    if (exportAll) {
        //
        ++level;
        //
        Icd::SystemPtrArray systems;
        if (!J_QPTR->parser()->parse(vehicle->id(), systems, Icd::ObjectSystem)) {
            return false;
        }
        Icd::SystemPtrArray::const_iterator citer = systems.cbegin();
        for (; citer != systems.cend(); ++citer) {
            const Icd::SystemPtr &system = *citer;
            if (!generateSystem(vehicle->id(), system, exportAll, level)) {
                return false;
            }
        }
    }

    return true;
}

bool WordGeneratorData::generateSystem(const QStandardItem *itemSystem, bool exportAll,
                                       bool rt, int level)
{
    if (!itemSystem) {
        return false;
    }

    //
    J_QPTR->parser()->setMessage(QStringLiteral("获取系统数据\n系统：%1")
                                .arg(itemSystem->text().remove(QRegExp("<([^>]*)>")))
                                .toStdString());

    //
    const QString domain = itemSystem->data(TreeItemDomainRole).toString();
    const std::string vehicleId = domain.section('/', 0, 0).toStdString();
    Icd::SystemPtr system;
    if (!J_QPTR->parser()->parse(vehicleId, domain.section('/', 1).toStdString(), system,
                                Icd::ObjectSystem)) {
        return false;
    }

    if (exportAll) {
        if (!generateSystem(vehicleId, system, exportAll, level)) {
            return false;
        }
    } else {
        if (!generateSystem(vehicleId, system, false, level)) {
            return false;
        }

        //
        ++level;

        //
        int count = itemSystem->rowCount();
        for (int i = 0; i < count; ++i) {
            const QStandardItem *itemTable = itemSystem->child(i);
            if (!itemTable) {
                continue;
            }
            if (!generateTable(itemTable, exportAll, rt, level)) {
                return false;
            }
        }
    }

    return true;
}

bool WordGeneratorData::generateSystem(const std::string &vehicleId, const SystemPtr &system,
                                       bool exportAll, int level)
{
    //
    J_QPTR->parser()->setMessage(QStringLiteral("生成系统文档\n系统：%1")
                                .arg(QString::fromStdString(system->name()))
                                .toStdString());

    //
    selection_->dynamicCall("EndKey(int)", 6);

    // 标题
    generateHeading(QString("%1 [%2]")
                    .arg(QString::fromStdString(system->name()))
                    .arg(QString::fromStdString(system->objectTypeString()).toUpper()), level);

    // 正文1
    QAxObject *paragraphs = selection_->querySubObject("Range")->querySubObject("Paragraphs");
    paragraphs->setProperty("OutlineLevel", 10);
    QAxObject *paragraphFormat = selection_->querySubObject("Range")->querySubObject("ParagraphFormat");
    paragraphFormat->setProperty("CharacterUnitFirstLineIndent", 2);
    QString systemDesc = QString::fromStdString(system->desc());
    if (!systemDesc.isEmpty() && !systemDesc.endsWith(QStringLiteral("。"))) {
        systemDesc.append(QStringLiteral("。"));
    }
    selection_->dynamicCall("TypeText(QString)", systemDesc);
    selection_->dynamicCall("TypeParagraph()");

    //
    if (exportAll) {
        //
        ++level;
        //
        J_QPTR->parser()->setMessage(QStringLiteral("获取所有表数据\n系统：%1")
                                    .arg(QString::fromStdString(system->name()))
                                    .toStdString());
        //
        Icd::TablePtrArray tables;
        if (!J_QPTR->parser()->parse(vehicleId, system->id(), tables, Icd::ObjectItem)) {
            return false;
        }
        Icd::TablePtrArray::const_iterator citer = tables.cbegin();
        for (; citer != tables.cend(); ++citer) {
            const Icd::TablePtr &table = *citer;
            if (!generateTable(table, level)) {
                return false;
            }
        }
    }

    return true;
}

bool WordGeneratorData::generateTable(const QStandardItem *itemTable, bool exportAll,
                                      bool rt, int level)
{
    if (!itemTable) {
        return false;
    }

    if (!exportAll && itemTable->rowCount() == 0) {
        return true;
    }

    //
    Icd::TablePtr table;
#if defined(ICDWORKER_LIB)
    const QVariant varChannelId = itemTable->data(Icd::TreeChannelIdRole);
    if (varChannelId.isValid()) {
        Icd::WorkerPtr worker = Icd::WorkerPool::getInstance()
                ->workerByChannelIdentity(varChannelId.toString().toStdString());
        if (worker) {
            if (rt) {
                table = worker->workerRecv()->table();
            } else {
                table = worker->workerSend()->table();
            }
        }
    }
#endif
    const QString domain = itemTable->data(Icd::TreeItemDomainRole).toString();
    if (!table) {
        //
        J_QPTR->parser()->setMessage(QStringLiteral("获取表数据\n表：%1")
                                    .arg(itemTable->text().remove(QRegExp("<([^>]*)>")))
                                    .toStdString());
        //
        if (!J_QPTR->parser()->parse(domain.section('/', 0, 0).toStdString(),
                                    domain.section('/', 1, 1).toStdString(),
                                    domain.section('/', 2, 2).toStdString(),
                                    table, Icd::ObjectItem) || !table) {
            return false;
        }
    }

    //
    Icd::TablePtr targetTable = table;
    if (itemTable->type() == Icd::TreeItemTypeItemTable) {
        targetTable = table->tableByDomain(domain.section('/', 3).toStdString());
        if (!targetTable) {
            return false;
        }
    }

    //
    if (!generateTable(targetTable, level)) {
        return false;
    }

    return true;
}

bool WordGeneratorData::generateTable(const Icd::TablePtr &table, int level)
{
    if (J_QPTR->parser()->canceledSaveAs()) {
        return false;
    }

    //
    QString domain = QString::fromStdString(table->name());
    J_QPTR->parser()->setMessage(QStringLiteral("生成表文档\n表：%1")
                                .arg(domain).toStdString());

    //
    selection_->dynamicCall("EndKey(int)", 6);

    // 标题
    generateHeading(QString("%1 [%2]")
                    .arg(QString::fromStdString(table->name()))
                    .arg(QString::fromStdString(table->typeName()).section('_', 0, 0).toUpper()),
                    level);

    // 正文1
    QAxObject *paragraphs = selection_->querySubObject("Range")->querySubObject("Paragraphs");
    paragraphs->setProperty("OutlineLevel", 10);
    QAxObject *paragraphFormat = selection_->querySubObject("Range")->querySubObject("ParagraphFormat");
    paragraphFormat->setProperty("CharacterUnitFirstLineIndent", 2);
    QString tableDesc = QString::fromStdString(table->desc());
    if (!tableDesc.isEmpty() && !tableDesc.endsWith(QStringLiteral("。"))) {
        tableDesc.append(QStringLiteral("。"));
    }
    if (!tableDesc.isEmpty()) {
        selection_->dynamicCall("TypeText(QString)", tableDesc);
        selection_->dynamicCall("TypeParagraph()");
    }
    // 正文2 - [1]
    selection_->dynamicCall("TypeText(QString)", QStringLiteral("具体信息见"));

    // 表单
    QAxObject *axTable = tables_->querySubObject(
                "Add(QVariant,int,int,int,int)",  selection_->querySubObject("Range")
                ->asVariant(), 1 + table->itemCount(), 5, 1, 2);
    axTable->setProperty("Style", QStringLiteral("网格表 1 浅色"));
    int tableSize = qCeil(table->bufferSize());
#if 1
    if (table->itemCount() == 0) {
        Icd::Object *parentObject = table->parent();
        if (parentObject && parentObject->objectType() == Icd::ObjectItem) {
            Icd::Item *parentItem = dynamic_cast<Icd::Item*>(parentObject);
            if (parentItem && parentItem->type() == Icd::ItemComplex) {
                Icd::ComplexItem *complexItem = dynamic_cast<Icd::ComplexItem*>(parentItem);
                if (complexItem) {
                    tableSize = qCeil(complexItem->bufferSize());
                }
            }
        }
    }
#endif
    generateTableCaption(QStringLiteral(" %1").arg(QString::fromStdString(table->name()))
                         .append(QStringLiteral("（"))
                         .append(QString::number(tableSize))
                         .append(QStringLiteral(" B）")), axTable);
    axTable->querySubObject("Columns(int)", 1)->setProperty("Width", 50);   // offset
    axTable->querySubObject("Columns(int)", 2)->setProperty("Width", 80);   // name
    axTable->querySubObject("Columns(int)", 3)->setProperty("Width", 50);   // type
    axTable->querySubObject("Columns(int)", 4)->setProperty("Width", 100);  // desc
    axTable->dynamicCall("AutoFitBehavior(int)", 2);

    // 正文2 - [2]
    selection_->dynamicCall("MoveUp(int,int)", 5, 1);
    selection_->dynamicCall("InsertCrossReference(QString,int,int,boolean,boolean,boolean,QString)",
                           QStringLiteral("表"), 3, tables_->property("Count").toInt(),
                           true, false, false, " ");
    selection_->dynamicCall("TypeText(QString)", QStringLiteral("所示。"));

    //
    axTable->querySubObject("Range")->dynamicCall("Select()");
    selection_->querySubObject("Cells")->setProperty("VerticalAlignment", 1);
    axTable->querySubObject("Rows(int)", 1)->querySubObject("Range")->dynamicCall("Select()");
    selection_->querySubObject("ParagraphFormat")->setProperty("Alignment", 1);
    setCellText(axTable, 1, 1, QStringLiteral("字节"));           // offset
    setCellText(axTable, 1, 2, QStringLiteral("名称"));           // name
    setCellText(axTable, 1, 3, QStringLiteral("类型"));           // type
    setCellText(axTable, 1, 4, QStringLiteral("特性"));           // feature
    setCellText(axTable, 1, 5, QStringLiteral("说明"));           // desc

    //
    ++level;

    // 表信息
    const Icd::ItemPtrArray &items = table->allItem();
    Icd::ItemPtrArray::const_iterator citer = items.cbegin();
    for (; citer != items.cend(); ++citer) {
        const Icd::ItemPtr &item = *citer;
        if (!generateItem(item, axTable, (citer - items.cbegin()) + 2, level)) {
            return false;
        }
    }

    return true;
}

bool WordGeneratorData::generateDataItem(const QStandardItem *itemData, bool exportAll,
                                         bool rt, int level)
{
    if (!itemData) {
        return false;
    }

    if (!exportAll && itemData->rowCount() == 0) {
        return true;
    }

    const QStandardItem *_itemTable = itemData;
    while (_itemTable) {
        if (_itemTable->type() <= Icd::TreeItemTypeTable) {
            break;
        }
        _itemTable = _itemTable->parent();
    }
    //
    Icd::TablePtr table;
#if defined(ICDWORKER_LIB)
    if (_itemTable && _itemTable->type() == Icd::TreeItemTypeTable) {
        const QVariant varChannelId = _itemTable->data(Icd::TreeChannelIdRole);
        if (varChannelId.isValid()) {
            Icd::WorkerPtr worker = Icd::WorkerPool::getInstance()
                    ->workerByChannelIdentity(varChannelId.toString().toStdString());
            if (worker) {
                if (rt) {
                    table = worker->workerRecv()->table();
                } else {
                    table = worker->workerSend()->table();
                }
            }
        }
    }
#endif
    const QString domain = itemData->data(Icd::TreeItemDomainRole).toString();
    if (!table) {
        //
        QString tableName;
        if (_itemTable && _itemTable->type() == Icd::TreeItemTypeTable) {
            tableName = _itemTable->text().remove(QRegExp("<([^>]*)>"));
        } else {
            tableName = QStringLiteral("<?>");
        }
        //
        J_QPTR->parser()->setMessage(QStringLiteral("获取表数据\n表：%1").arg(tableName)
                                    .toStdString());
        //
        if (!J_QPTR->parser()->parse(domain.section('/', 0, 0).toStdString(),
                                    domain.section('/', 1, 1).toStdString(),
                                    domain.section('/', 2, 2).toStdString(),
                                    table, Icd::ObjectItem) || !table) {
            return false;
        }
    }

    //
    Icd::ObjectPtr targetObject = table->itemByDomain(domain.section('/', 3).toStdString());
    if (!targetObject || targetObject->objectType() != Icd::ObjectItem) {
        return false;
    }

    const Icd::ItemPtr item = JHandlePtrCast<Icd::Item, Icd::Object>(targetObject);
    if (!item) {
        return false;
    }

    //
    if (!generateDataItem(item, level)) {
        return false;
    }

    return true;
}

bool WordGeneratorData::generateDataItem(const Icd::ItemPtr &item, int level)
{
    if (!item) {
        return false;
    }

    if (J_QPTR->parser()->canceledSaveAs()) {
        return false;
    }

    //
    QString domain = QString::fromStdString(item->name());
    J_QPTR->parser()->setMessage(QStringLiteral("生成数据文档\n数据项：%1")
                                .arg(domain).toStdString());

    //
    selection_->dynamicCall("EndKey(int)", 6);

    // 标题
    generateHeading(QString("%1 [%2]")
                    .arg(QString::fromStdString(item->name()))
                    .arg(QString::fromStdString(item->typeName()).section('_', 0, 0).toUpper()), level);

    // 正文1
    QAxObject *paragraphs = selection_->querySubObject("Range")->querySubObject("Paragraphs");
    paragraphs->setProperty("OutlineLevel", 10);
    QAxObject *paragraphFormat = selection_->querySubObject("Range")->querySubObject("ParagraphFormat");
    paragraphFormat->setProperty("CharacterUnitFirstLineIndent", 2);
    QString itemDesc = QString::fromStdString(item->desc());
    if (!itemDesc.isEmpty() && !itemDesc.endsWith(QStringLiteral("。"))) {
        itemDesc.append(QStringLiteral("。"));
    }
    if (!itemDesc.isEmpty()) {
        selection_->dynamicCall("TypeText(QString)", itemDesc);
        selection_->dynamicCall("TypeParagraph()");
    }
    // 正文2 - [1]
    selection_->dynamicCall("TypeText(QString)", QStringLiteral("具体信息见"));

    // 表单
    QAxObject *axTable = tables_->querySubObject(
                "Add(QVariant,int,int,int,int)",  selection_->querySubObject("Range")
                ->asVariant(), 2, 5, 1, 2);
    int itemSize = qCeil(item->bufferSize());
    generateTableCaption(QStringLiteral(" %1").arg(QString::fromStdString(item->name()))
                         .append(QStringLiteral("（"))
                         .append(QString::number(itemSize))
                         .append(QStringLiteral(" B）")), axTable);
    axTable->querySubObject("Columns(int)", 1)->setProperty("Width", 50);   // offset
    axTable->querySubObject("Columns(int)", 2)->setProperty("Width", 80);   // name
    axTable->querySubObject("Columns(int)", 3)->setProperty("Width", 50);   // type
    axTable->querySubObject("Columns(int)", 4)->setProperty("Width", 100);  // desc
    axTable->dynamicCall("AutoFitBehavior(int)", 2);

    // 正文2 - [2]
    selection_->dynamicCall("MoveUp(int,int)", 5, 1);
    selection_->dynamicCall("InsertCrossReference(QString,int,int,boolean,boolean,boolean,QString)",
                           QStringLiteral("表"), 3, tables_->property("Count").toInt(),
                           true, false, false, " ");
    selection_->dynamicCall("TypeText(QString)", QStringLiteral("所示。"));

    //
    axTable->querySubObject("Range")->dynamicCall("Select()");
    selection_->querySubObject("Cells")->setProperty("VerticalAlignment", 1);
    axTable->querySubObject("Rows(int)", 1)->querySubObject("Range")->dynamicCall("Select()");
    selection_->querySubObject("ParagraphFormat")->setProperty("Alignment", 1);
    setCellText(axTable, 1, 1, QStringLiteral("字节"));       // offset
    setCellText(axTable, 1, 2, QStringLiteral("名称"));       // name
    setCellText(axTable, 1, 3, QStringLiteral("类型"));       // type
    setCellText(axTable, 1, 4, QStringLiteral("特性"));       // feature
    setCellText(axTable, 1, 5, QStringLiteral("说明"));       // desc

    //
    ++level;

    // 数据项信息
    if (!generateItem(item, axTable, 2, level)) {
        return false;
    }

    return true;
}

bool WordGeneratorData::generateContents()
{
    selection_->dynamicCall("HomeKey(int)", 6);
    selection_->dynamicCall("InsertBreak(int)", 7);
    selection_->dynamicCall("MoveUp(int)", 5);
    selection_->querySubObject("Paragraphs")->setProperty("Alignment", 1);
    selection_->querySubObject("Font")->setProperty("Bold", 1);
    selection_->dynamicCall("TypeText(QString)", QStringLiteral("目录"));
    selection_->dynamicCall("TypeParagraph()");
    selection_->querySubObject("Paragraphs")->dynamicCall("Reset()");
    selection_->querySubObject("Font")->dynamicCall("Reset()");
    selection_->querySubObject("Paragraphs")->setProperty("OutlineLevel", 10);
    QVariantList params;
    params << selection_->querySubObject("Range")->asVariant() << true
           << 1 << 9 << true << "C" << true << true << "" << true <<  true << true;
    document_->querySubObject("TablesOfContents")->dynamicCall(
                "Add(QVariant,boolean,int,int,boolean,QString,boolean,"
                "boolean,QString,boolean,boolean,boolean)", params);

    return true;
}

bool WordGeneratorData::generateHeading(const QString &text, int level)
{
    // 标题
    QAxObject *paragraphs = selection_->querySubObject("Range")->querySubObject("Paragraphs");
    paragraphs->setProperty("OutlineLevel", level);
    QAxObject *listFormat = selection_->querySubObject("Range")->querySubObject("ListFormat");
    listFormat->dynamicCall("ApplyListTemplateWithLevel(QVariant,boolean,int,int,int)",
                            listTemplate_->asVariant(), false, 2, 2);
    QAxObject *font = selection_->querySubObject("Font");
    font->setProperty("Bold", 1);
    font->setProperty("Size", 12);
    //font->setProperty("Name", QStringLiteral("宋体"));
    selection_->dynamicCall("TypeText(QString)", text);
    paragraphs->dynamicCall("Reset()");
    selection_->dynamicCall("TypeParagraph()");

    return true;
}

bool WordGeneratorData::generateTableCaption(const QString &title, QAxObject *tableObject)
{
    QAxObject *tableRange = tableObject->querySubObject("Range");
    tableRange->dynamicCall("InsertCaption(QVariant,QString)", tableCaption_->asVariant(), title);
    QAxObject *paragraphs = selection_->querySubObject("Range")->querySubObject("Paragraphs");
    paragraphs->setProperty("Alignment", 1);

    return true;
}

bool WordGeneratorData::generateItem(const Icd::ItemPtr &item, QAxObject *axTable,
                                     int row, int level)
{
    if (J_QPTR->parser()->canceledSaveAs()) {
        return false;
    }

    switch (item->type()) {
    case Icd::ItemHead:
        return generateHeaderItem(JHandlePtrCast<Icd::HeaderItem, Icd::Item>(item), axTable, row);
    case Icd::ItemCounter:
        return generateCounterItem(JHandlePtrCast<Icd::CounterItem, Icd::Item>(item), axTable, row);
    case Icd::ItemCheck:
        return generateCheckItem(JHandlePtrCast<Icd::CheckItem, Icd::Item>(item), axTable, row);
    case Icd::ItemFrameCode:
        return generateFrameCodeItem(JHandlePtrCast<Icd::FrameCodeItem, Icd::Item>(item), axTable, row);
    case Icd::ItemNumeric:
        return generateNumericItem(JHandlePtrCast<Icd::NumericItem, Icd::Item>(item), axTable, row);
    case Icd::ItemBitMap:
    case Icd::ItemBitValue:
        return generateBitItem(JHandlePtrCast<Icd::BitItem, Icd::Item>(item), axTable, row);
    case Icd::ItemComplex:
        return generateComplexItem(JHandlePtrCast<Icd::ComplexItem, Icd::Item>(item), axTable, row, level);
    case Icd::ItemFrame:
        return generateFrameItem(JHandlePtrCast<Icd::FrameItem, Icd::Item>(item), axTable, row, level);
    case Icd::ItemDateTime:
        return generateDateTimeItem(JHandlePtrCast<Icd::DateTimeItem, Icd::Item>(item), axTable, row);
    case Icd::ItemArray:
        return generateArrayItem(JHandlePtrCast<Icd::ArrayItem, Icd::Item>(item), axTable, row);
    default:
        return false;
    }

    if (J_QPTR->parser()->canceledSaveAs()) {
        return false;
    }

    return true;
}

bool WordGeneratorData::generateHeaderItem(const HeaderItemPtr &headerItem, QAxObject *axTable, int row)
{
    int offset = static_cast<int>(headerItem->bufferOffset());
    int size = qCeil(headerItem->bufferSize());
    // column 1 - offset
    if (size > 1) {
        setCellText(axTable, row, 1, QString("%1 ~ %2").arg(offset).arg(offset + size - 1));
    } else {
        setCellText(axTable, row, 1, offset);
    }
    // column 2 - name
    setCellText(axTable, row, 2, QString::fromStdString(headerItem->name()));
    // column 3 - type
    setCellText(axTable, row, 3, QString::fromStdString(headerItem->typeName()).section('_', 1));
    // column 4 - feature
    //setCellText(axTable, row, 4, QStringLiteral("无"));
    // column 5 - desc
    setCellText(axTable, row, 5, QString::fromStdString(headerItem->desc()));

    return true;
}

bool WordGeneratorData::generateCounterItem(const CounterItemPtr &counterItem, QAxObject *axTable, int row)
{
    int offset = static_cast<int>(counterItem->bufferOffset());
    int size = qCeil(counterItem->bufferSize());
    // column 1 - offset
    if (size > 1) {
        setCellText(axTable, row, 1, QString("%1 ~ %2").arg(offset).arg(offset + size - 1));
    } else {
        setCellText(axTable, row, 1, offset);
    }
    // column 2 - name
    setCellText(axTable, row, 2, QString::fromStdString(counterItem->name()));
    // column 3 - type
    setCellText(axTable, row, 3, QString::fromStdString(counterItem->typeName()).section('_', 1));
    // column 4 - feature
    //setCellText(axTable, row, 4, QStringLiteral("无"));
    // column 5 - desc
    setCellText(axTable, row, 5, QString::fromStdString(counterItem->desc()));

    return true;
}

bool WordGeneratorData::generateCheckItem(const CheckItemPtr &checkItem, QAxObject *axTable, int row)
{
    int offset = static_cast<int>(checkItem->bufferOffset());
    int size = qCeil(checkItem->bufferSize());
    // column 1 - offset
    if (size > 1) {
        setCellText(axTable, row, 1, QString("%1 ~ %2").arg(offset).arg(offset + size - 1));
    } else {
        setCellText(axTable, row, 1, offset);
    }
    // column 2 - name
    setCellText(axTable, row, 2, QString::fromStdString(checkItem->name()));
    // column 3 - type
    setCellText(axTable, row, 3, QString::fromStdString(checkItem->typeName()).section('_', 1));
    // column 4 - feature
    //setCellText(axTable, row, 4, QStringLiteral("无"));
    // column 5 - desc
    setCellText(axTable, row, 5, QString::fromStdString(checkItem->desc()));

    return true;
}

bool WordGeneratorData::generateFrameCodeItem(const FrameCodeItemPtr &frameCodeItem, QAxObject *axTable, int row)
{
    int offset = static_cast<int>(frameCodeItem->bufferOffset());
    int size = qCeil(frameCodeItem->bufferSize());
    // column 1 - offset
    if (size > 1) {
        setCellText(axTable, row, 1, QString("%1 ~ %2").arg(offset).arg(offset + size - 1));
    } else {
        setCellText(axTable, row, 1, offset);
    }
    // column 2 - name
    setCellText(axTable, row, 2, QString::fromStdString(frameCodeItem->name()));
    // column 3 - type
    setCellText(axTable, row, 3, QString::fromStdString(frameCodeItem->typeName()).section('_', 1));
    // column 4 - feature
    //setCellText(axTable, row, 4, QStringLiteral("无"));
    // column 5 - desc
    setCellText(axTable, row, 5, QString::fromStdString(frameCodeItem->desc()));

    return true;
}

bool WordGeneratorData::generateNumericItem(const NumericItemPtr &numericItem, QAxObject *axTable, int row)
{
    int offset = static_cast<int>(numericItem->bufferOffset());
    int size = qCeil(numericItem->bufferSize());
    // column 1 - offset
    if (size > 1) {
        setCellText(axTable, row, 1, QString("%1 ~ %2").arg(offset).arg(offset + size - 1));
    } else {
        setCellText(axTable, row, 1, offset);
    }
    // column 2 - name
    setCellText(axTable, row, 2, QString::fromStdString(numericItem->name()));
    // column 3 - type
    setCellText(axTable, row, 3, QString::fromStdString(numericItem->typeName()).section('_', 1));
    // column 4 - feature
    setCellText(axTable, row, 4,
                QStringLiteral("1）单位：%1；\n2）比例：%2；\n3）偏移：%3；\n4）范围：%4。")
                .arg(numericItem->unit().empty() ? "<?>"
                                                 : QString::fromStdString(numericItem->unit()))
                .arg(numericItem->scale())
                .arg(numericItem->offset())
                .arg(QString::fromStdString(numericItem->limit()->toString())));
    // column 5 - desc
    setCellText(axTable, row, 5, QString::fromStdString(numericItem->desc()));

    return true;
}

bool WordGeneratorData::generateBitItem(const BitItemPtr &bitItem, QAxObject *axTable, int row)
{
    int offset = static_cast<int>(bitItem->bufferOffset());
    int size = qCeil(bitItem->typeSize());
    // column 1 - offset
    if (size > 1) {
        setCellText(axTable, row, 1, QString("%1 ~ %2").arg(offset).arg(offset + size - 1));
    } else {
        setCellText(axTable, row, 1, offset);
    }
    // column 2 - name
    setCellText(axTable, row, 2, QString::fromStdString(bitItem->name()));
    // column 3 - type
    setCellText(axTable, row, 3, QString::fromStdString(bitItem->typeName()).section('_', 1));
    // column 4 - feature
    QString feature = QStringLiteral("1）位域：[%1，%2]。")
            .arg(bitItem->bitStart())
            .arg(bitItem->bitStart() + bitItem->bitCount() - 1);
    const std::map<icd_uint64, std::string> &specs = bitItem->specs();
    if (!specs.empty()) {
        feature.append(QStringLiteral("\n2）特征值：\n"));
    }
    std::map<icd_uint64, std::string>::const_iterator citerSpecs = specs.cbegin();
    switch (bitItem->type()) {
    case Icd::ItemBitMap:
    {
        for (; citerSpecs != specs.cend(); ++citerSpecs) {
            feature.append(QStringLiteral(" BIT%1：%2；\n").arg(citerSpecs->first)
                           .arg(citerSpecs->second.empty() ? "<?>"
                                                           : QString::fromStdString(citerSpecs->second)));
        }
        break;
    }
    case Icd::ItemBitValue:
    {
        for (; citerSpecs != specs.cend(); ++citerSpecs) {
            feature.append(QStringLiteral(" %1：%2；\n").arg(citerSpecs->first)
                           .arg(citerSpecs->second.empty() ? "<?>"
                                                           : QString::fromStdString(citerSpecs->second)));
        }
        break;
    }
    default:
        break;
    }
    if (!specs.empty()) {
        feature.chop(2);
    }
    setCellText(axTable, row, 4, feature);
    // column 5 - desc
    setCellText(axTable, row, 5, QString::fromStdString(bitItem->desc()));

    return true;
}

bool WordGeneratorData::generateComplexItem(const ComplexItemPtr &complexItem, QAxObject *axTable,
                                            int row, int level)
{
    const Icd::TablePtr table = complexItem->table();
    if (!table) {
        return true;    // FIXME
    }

    //
    QVariant headings = document_->dynamicCall("GetCrossReferenceItems(int)", 1);
    if (headings.isNull() || headings.type() != QVariant::StringList) {
        return false;
    }
    int headingLength = headings.toStringList().count();

    if (!generateTable(table, level)) {
        return false;
    }

    int offset = static_cast<int>(complexItem->bufferOffset());
    int size = qCeil(complexItem->bufferSize());
    // column 1 - offset
    if (size > 1) {
        setCellText(axTable, row, 1, QString("%1 ~ %2").arg(offset).arg(offset + size - 1));
    } else {
        setCellText(axTable, row, 1, offset);
    }
    // column 2 - name
    setCellText(axTable, row, 2, QString::fromStdString(complexItem->name()));
    // column 3 - type
    setCellText(axTable, row, 3, QStringLiteral("复合/%1B").arg(complexItem->bufferSize()));
    // column 4 - feature
    //setCellText(axTable, row, 4, QStringLiteral("无"));
    // column 5 - desc
    QString desc = QString::fromStdString(complexItem->desc());
    if (!desc.isEmpty() && !desc.endsWith(QStringLiteral("。"))) {
        desc.append(QStringLiteral("。"));
    }
    setCellText(axTable, row, 5, desc);
    // reference
    axTable->querySubObject("Cell(int,int", row, 5)->dynamicCall("Select()");
    selection_->dynamicCall("TypeText(QString)", QStringLiteral("（见"));
    selection_->dynamicCall("InsertCrossReference(QString,int,int,boolean)",
                           QStringLiteral("标题"), -2, headingLength + 1, true);
    selection_->dynamicCall("TypeText(QString)", QStringLiteral("）"));

    return true;
}

bool WordGeneratorData::generateFrameItem(const FrameItemPtr &frameItem, QAxObject *axTable,
                                          int row, int level)
{
    selection_->dynamicCall("EndKey(int)", 6);

    // 标题
    generateHeading(QString("%1 [%2]")
                    .arg(QString::fromStdString(frameItem->name()))
                    .arg(QString::fromStdString(frameItem->typeName()).section('_', 0, 0).toUpper()),
                    level);
    //
    QVariant headings = document_->dynamicCall("GetCrossReferenceItems(int)", 1);
    if (headings.isNull() || headings.type() != QVariant::StringList) {
        return false;
    }

    int headingLength = headings.toStringList().count();

    // sub-table
    {
        const Icd::TablePtrMap &tables = frameItem->allTable();

        // 正文1
        QAxObject *paragraphs = selection_->querySubObject("Range")->querySubObject("Paragraphs");
        paragraphs->setProperty("OutlineLevel", 10);
        QAxObject *paragraphFormat = selection_->querySubObject("Range")->querySubObject("ParagraphFormat");
        paragraphFormat->setProperty("CharacterUnitFirstLineIndent", 2);
        QString frameDesc = QString::fromStdString(frameItem->desc());
        if (!frameDesc.isEmpty() && !frameDesc.endsWith(QStringLiteral("。"))) {
            frameDesc.append(QStringLiteral("。"));
        }
        if (!frameDesc.isEmpty()) {
            selection_->dynamicCall("TypeText(QString)", frameDesc);
            selection_->dynamicCall("TypeParagraph()");
        }
        // 正文2 - [1]
        selection_->dynamicCall("TypeText(QString)", QStringLiteral("具体信息见"));

        // 表单
        QAxObject *axSubTable = tables_->querySubObject(
                    "Add(QVariant,int,int,int,int)",  selection_->querySubObject("Range")
                    ->asVariant(), 1 + tables.size(), 5, 1, 2);
        axSubTable->setProperty("Style", QStringLiteral("网格表 1 浅色"));
        int frameSize = qCeil(frameItem->bufferSize());
        generateTableCaption(QStringLiteral(" %1").arg(QString::fromStdString(frameItem->name()))
                             .append(QStringLiteral("（"))
                             .append(QString::number(frameSize))
                             .append(QStringLiteral(" B）")), axSubTable);
        axSubTable->querySubObject("Columns(int)", 1)->setProperty("Width", 50);    // code
        axSubTable->querySubObject("Columns(int)", 2)->setProperty("Width", 120);   // name
        axSubTable->querySubObject("Columns(int)", 3)->setProperty("Width", 50);    // size
        axSubTable->querySubObject("Columns(int)", 4)->setProperty("Width", 50);    // sequence
        axSubTable->querySubObject("Columns(int)", 5)->setProperty("Width", 100);   // desc
        axSubTable->dynamicCall("AutoFitBehavior(int)", 2);

        // 正文2 - [2]
        selection_->dynamicCall("MoveUp(int,int)", 5, 1);
        selection_->dynamicCall("InsertCrossReference(QString,int,int,boolean,boolean,boolean,QString)",
                               QStringLiteral("表"), 3, tables_->property("Count").toInt(),
                               true, false, false, " ");
        selection_->dynamicCall("TypeText(QString)", QStringLiteral("所示。"));

        //
        axSubTable->querySubObject("Range")->dynamicCall("Select()");
        selection_->querySubObject("Cells")->setProperty("VerticalAlignment", 1);
        axSubTable->querySubObject("Rows(int)", 1)->querySubObject("Range")->dynamicCall("Select()");
        selection_->querySubObject("ParagraphFormat")->setProperty("Alignment", 1);
        setCellText(axSubTable, 1, 1, QStringLiteral("帧码"));        // code
        setCellText(axSubTable, 1, 2, QStringLiteral("名称"));        // name
        setCellText(axSubTable, 1, 3, QStringLiteral("大小"));        // size
        setCellText(axSubTable, 1, 4, QStringLiteral("时序"));        // sequence
        setCellText(axSubTable, 1, 5, QStringLiteral("说明"));        // desc

        //
        ++level;

        int row = 2;
        Icd::TablePtrMap::const_iterator citer = tables.cbegin();
        for (; citer != tables.cend(); ++citer, ++row) {
            const Icd::TablePtr &table = citer->second;
            if (!generateSubTable(table, axSubTable, row, level)) {
                return false;
            }
        }
    }

    int offset = static_cast<int>(frameItem->bufferOffset());
    int size = qCeil(frameItem->bufferSize());
    // column 1 - offset
    if (size > 1) {
        setCellText(axTable, row, 1, QString("%1 ~ %2").arg(offset).arg(offset + size - 1));
    } else {
        setCellText(axTable, row, 1, offset);
    }
    // column 2 - name
    setCellText(axTable, row, 2, QString::fromStdString(frameItem->name()));
    // column 3 - type
    setCellText(axTable, row, 3, QStringLiteral("子帧/%1B").arg(frameItem->bufferSize()));
    // column 4 - feature
    //setCellText(axTable, row, 4, QStringLiteral("无"));
    // column 5 - desc
    QString desc = QString::fromStdString(frameItem->desc());
    if (!desc.isEmpty() && !desc.endsWith(QStringLiteral("。"))) {
        desc.append(QStringLiteral("。"));
    }
    setCellText(axTable, row, 5, desc);
    // reference
    axTable->querySubObject("Cell(int,int", row, 5)->dynamicCall("Select()");
    selection_->dynamicCall("TypeText(QString)", QStringLiteral("（见"));
    selection_->dynamicCall("InsertCrossReference(QString,int,int,boolean)",
                           QStringLiteral("标题"), -2, headingLength, true);
    selection_->dynamicCall("TypeText(QString)", QStringLiteral("）"));

    return true;
}

bool WordGeneratorData::generateSubTable(const TablePtr &table, QAxObject *axTable,
                                         int row, int level)
{
    if (!table || !axTable) {
        return false;
    }
    //
    QVariant headings = document_->dynamicCall("GetCrossReferenceItems(int)", 1);
    if (headings.isNull() || headings.type() != QVariant::StringList) {
        return false;
    }

    int headingLength = headings.toStringList().count();

    if (!generateTable(table, level)) {
        return false;
    }

    // column 1 - code
    setCellText(axTable, row, 1, QString::fromStdString(table->mark()));
    // column 2 - name
    setCellText(axTable, row, 2, QString::fromStdString(table->name()));
    // column 3 - size
    setCellText(axTable, row, 3, QStringLiteral("%1B").arg(table->bufferSize()));
    // column 4 - sequence
    setCellText(axTable, row, 4, QStringLiteral("%1").arg(table->sequence()));
    // column 5 - desc
    QString desc = QString::fromStdString(table->desc());
    if (!desc.isEmpty() && !desc.endsWith(QStringLiteral("。"))) {
        desc.append(QStringLiteral("。"));
    }
    setCellText(axTable, row, 5, desc);
    // reference
    axTable->querySubObject("Cell(int,int", row, 5)->dynamicCall("Select()");
    selection_->dynamicCall("TypeText(QString)", QStringLiteral("（见"));
    selection_->dynamicCall("InsertCrossReference(QString,int,int,boolean)",
                           QStringLiteral("标题"), -2, headingLength + 1, true);
    selection_->dynamicCall("TypeText(QString)", QStringLiteral("）"));

    return true;
}

bool WordGeneratorData::generateDateTimeItem(const DateTimeItemPtr &dateTimeItem,
                                             QAxObject *axTable, int row)
{
    Q_UNUSED(dateTimeItem);
    Q_UNUSED(axTable);
    Q_UNUSED(row);
    Q_ASSERT(false);        //TODO [not supported]
    return false;
}

bool WordGeneratorData::generateArrayItem(const ArrayItemPtr &arrayItem,
                                          QAxObject *axTable, int row)
{
    if (!arrayItem) {
        return false;
    }

    int offset = static_cast<int>(arrayItem->bufferOffset());
    int size = qCeil(arrayItem->bufferSize());
    // column 1 - offset
    if (size > 1) {
        setCellText(axTable, row, 1, QString("%1 ~ %2").arg(offset).arg(offset + size - 1));
    } else {
        setCellText(axTable, row, 1, offset);
    }
    // column 2 - name
    setCellText(axTable, row, 2, QString::fromStdString(arrayItem->name()));
    // column 3 - type
    setCellText(axTable, row, 3, QString::fromStdString(arrayItem->typeName())
                .section('_', 1).append("[]"));
    // column 4 - feature
    setCellText(axTable, row, 4, QStringLiteral("%1个元素").arg(arrayItem->count()));
    // column 5 - desc
    setCellText(axTable, row, 5, QString::fromStdString(arrayItem->desc()));

    return true;
}

bool WordGeneratorData::setCellText(QAxObject *axTable, int row, int column, const QVariant &text)
{
    axTable->querySubObject("Cell(int,int)", row, column)->querySubObject("Range")
            ->setProperty("Text", text);

    return true;
}

} // end of namespace Icd
