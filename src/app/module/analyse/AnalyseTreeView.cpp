#include "precomp.h"
#include "AnalyseTreeView.h"
#include "tablesel_widget.h"
#include "axissel_widget.h"
#include "private/ExportDataDlg.h"

namespace Analyse {

TreeView::TreeView(QWidget *parent)
    : QWidget(parent)
{
    QVBoxLayout *vertLyoutMain = new QVBoxLayout(this);
    vertLyoutMain->setContentsMargins(0, 0, 0, 0);

    d_treeView = new Icd::CoreTreeWidget(this);
    d_treeView->setTreeMode(Icd::CoreTreeWidget::TreeModeAnalyse);
    vertLyoutMain->addWidget(d_treeView);

    jnotify->on("analyse.toolbar.database.config", this, [=](JNEvent &){
        QVariantList args;
        args << "analyse" << qVariantFromValue((void*)this);
        jnotify->send("database.config", args);
    });
    connect(d_treeView, &Icd::CoreTreeWidget::itemPressed, this, [=](QStandardItem *item){
        jnotify->send("analyse.tree.item.pressed", qVariantFromValue((void*)item));
    });
    connect(d_treeView, &Icd::CoreTreeWidget::itemClicked, this, [=](QStandardItem *item){
        QVariantList args;
        args.append(qVariantFromValue((void*)item));
        QStandardItem *itemTable = d_treeView->findItemTable(item);
        args.append(qVariantFromValue((void*)itemTable));
        jnotify->send("analyse.tree.item.clicked", args);
    });
    connect(d_treeView, &Icd::CoreTreeWidget::currentItemChanged, this,
            [=](QStandardItem *current, QStandardItem *previous){
        QVariantList args;
        args.append(qVariantFromValue((void*)current));
        args.append(qVariantFromValue((void*)previous));
        jnotify->send("analyse.tree.item.currentchanged", args);
    });
    connect(d_treeView, &Icd::CoreTreeWidget::itemUnloaded, this,
            [=](QStandardItem *item, QStandardItem *tableItem){
        QVariantList args;
        args.append(qVariantFromValue((void*)item));
        args.append(qVariantFromValue((void*)tableItem));
        jnotify->send("analyse.tree.item.unloaded", args);
    });
    connect(d_treeView, &Icd::CoreTreeWidget::unbindItem, this,
            [=](QStandardItem *item, QStandardItem *tableItem){
        QVariantList args;
        args.append(qVariantFromValue((void*)item));
        args.append(qVariantFromValue((void*)tableItem));
        jnotify->send("analyse.tree.item.unbind", args);
    });
    connect(d_treeView, &Icd::CoreTreeWidget::exportAnalyseData, this,
            [=](QStandardItem *item, const QString &filePath,
            bool hasTimeFormat, int headerSize){
        exportData(item, filePath, hasTimeFormat, headerSize);
    });

    jnotify->on("analyse.parser.changed", this, [=](JNEvent &){
        if (!updateParser()) {
            qDebug() << "analyse: update parser failure!";
        }
    });
    jnotify->on("analyse.parser.inst", this, [=](JNEvent &event){
        Icd::JParserPtrHandle *handle =
                jVariantFromVoid<Icd::JParserPtrHandle>(event.argument());
        if (!handle) {
            return;
        }
        handle->parser = d_treeView->parser();
        event.setReturnValue(true);
    });
    jnotify->on("analyse.toolbar.database.config", this, [=](JNEvent &){
        QVariantList args;
        // module
        args.append("analyse");
        // receiver
        args.append(qVariantFromValue((void*)this));
        jnotify->send("database.config", args);
    });
    jnotify->on("analyse.toolbar.tree.showOffset", this, [=](JNEvent &event){
        const bool checked = event.argument().toBool();
        d_treeView->setShowAttribute(Icd::CoreTreeWidget::ShowOffset, checked);
        JMain::instance()->setOption("analyse", "option.tree.showOffset", checked);
    });
    jnotify->on("analyse.toolbar.tree.showType", this, [=](JNEvent &event){
        const bool checked = event.argument().toBool();
        d_treeView->setShowAttribute(Icd::CoreTreeWidget::ShowType, checked);
        JMain::instance()->setOption("analyse", "option.tree.showType", checked);
    });
    jnotify->on("analyse.toolbar.tree.loadData", this, [=](JNEvent &){
        loadRecordData();
    });
    jnotify->on("analyse.toolbar.tree.unloadData", this, [=](JNEvent &){
        unloadRecordData();
    });
}

TreeView::~TreeView()
{

}

bool TreeView::init()
{
    bool result = true;

    Json::Value option = JMain::instance()->option("analyse", "option.tree");
    if (option.isNull()) {
        return false;
    }

    if (!updateParser()) {
        qDebug() << "analyse: update parser failure!";
        return false;
    }

    return result;
}

void TreeView::setShowAttribute(int attr, bool on)
{
    d_treeView->setShowAttribute((Icd::CoreTreeWidget::ShowAttribute)attr, on);
}

void TreeView::loadRecordData()
{
    QString selectedFilter = tr("Any file (*.*)");
    const QString filePath = QFileDialog::getOpenFileName(
                this, tr("Load data"), QString(),
                tr("Any file (*.*);;"
                "ICD file (*.icd);;"
                "Data file (*.dat);;"
                "Text file (*.txt)"), &selectedFilter);
    if (filePath.isEmpty()) {
        return;
    }

    JAutoCursor cursor(Qt::BusyCursor);

    //
    if (!parseDataFile(filePath)) {
        return;
    }
}

void TreeView::unloadRecordData()
{
    d_treeView->clearContents();
    d_table = Icd::TablePtr();
    d_fileTables.clear();
    d_tempFiles.clear();
}

bool TreeView::updateParser()
{
    d_treeView->clearContents();

    const Json::Value config = JMain::instance()->option("analyse", "parser");
    if (config.isNull()) {
        return false;
    }

    const Icd::ParserPtr parser = Icd::Parser::create(config);
    if (!parser) {
        return false;
    }

    d_treeView->setParser(parser);

    d_treeView->clearContents();

    return true;
}

bool TreeView::parseDataFile(const QString &filePath)
{
    QFile file(filePath);
    if (!file.open(QFile::ReadOnly)) {
        return false;
    }

    // [filetype:4B][attr:2B][domain:>=1B]
    if (file.size() < 7) {
        return false;
    }

    bool hasTimeFormat = false;
    int headerSize = 0;
    QString domain;

    // 判断文件类型
    const QString header = QString(file.read(4));
    if (header == "ICD") {
        headerSize += 4;
        const QByteArray attr = file.read(2);
        if (attr.size() != 2) {
            return false;
        }
        headerSize += 2;
        if (attr.at(0) == 'd') {
            domain = QString::fromLocal8Bit(file.readLine(256)).trimmed();
            headerSize += domain.size() + 1;
        }
        if (attr.at(1) == 't') {
            hasTimeFormat = true;
        }
    }

    file.close();

    if (domain.isEmpty()) {
        TableSelWidget tableSelWidget(this);
        tableSelWidget.resize(600, 660);
        if (!tableSelWidget.init()) {
            return false;
        }
        if (tableSelWidget.exec() != QDialog::Accepted) {
            return false;
        }
        domain = tableSelWidget.tableDomain();
        if (domain.isEmpty()) {
            return false;
        }
    }

    //
    if (!loadData(domain, filePath, hasTimeFormat, headerSize)) {
        return false;
    }

    return true;
}

bool TreeView::loadData(const QString &domain, const QString &filePath,
                        bool hasTimeFormat, int headerSize)
{
    bool result = true;

    const Icd::ParserPtr parser = d_treeView->parser();
    if (!parser) {
        return false;
    }

    Icd::ProgressDialog *progressDialog = new Icd::ProgressDialog(this);
    progressDialog->setWindowTitle(tr("Load data"));
    progressDialog->setProgressValue(100);
    progressDialog->setCancelVisible(false);
    progressDialog->setMessage(tr("Parsing data file..."));
    QFuture<bool> future = QtConcurrent::run([=]() -> bool {
        if (!parser->parse(domain.section('/', 0, 0).toStdString(),
                           domain.section('/', 1, 1).toStdString(),
                           domain.section('/', 2).toStdString(),
                           d_table, Icd::ObjectItem)) {
            return false;
        }
        return true;
    });
    connect(progressDialog, &Icd::ProgressDialog::finished, this, [=](){
        bool success = progressDialog->futureResult();
        if (success) {
            QSharedPointer<QTemporaryFile> tempFile;
            // parity
            if (headerSize <= 0) {
                tempFile = QSharedPointer<QTemporaryFile>(new QTemporaryFile(filePath));
                if (tempFile->open()) {
                    QFile sourceFile(filePath);
                    if (sourceFile.open(QFile::ReadOnly)) {
                        // parity
                        if (!Icd::checkData(d_table, headerSize, &sourceFile, tempFile.data())) {
                            success = false;
                        }
                    } else {
                        success = false;
                    }
                    if (tempFile->size() == 0) {
                        progressDialog->hide();
                        QMessageBox::warning(this, tr("Warning"),
                                             tr("Parse failure! There're no valid data."));
                        success = false;
                    }
                    tempFile->close();
                } else {
                    success = false;
                }
            }
            //
            if (success) {
                QString newFilePath;
                if (tempFile) {
                    newFilePath = tempFile->fileName();
                } else {
                    newFilePath = filePath;
                }
                if (d_treeView->loadData(d_table, newFilePath, hasTimeFormat, headerSize, domain)) {
                    d_fileTables[newFilePath] = d_table;
                    if (tempFile) {
                        d_tempFiles[newFilePath] = tempFile;
                    }
                    struct { Icd::TablePtr table; } data;
                    data.table = d_table;
                    QVariantList args;
                    args.append(newFilePath);
                    args.append(qVariantFromValue((void*)&data));
                    jnotify->send("analyse.tableLoaded", args);
                }
                d_table = Icd::TablePtr();
            }
        }
        progressDialog->disconnect(this);
        if (!progressDialog->isHidden()) {
            QString message;
            if (success) {
                message = tr("Parse success");
            } else {
                message = tr("Parse failure!");
            }
            progressDialog->hide();
            QMessageBox::information(this, tr("Parsing result"), message);
        }
        progressDialog->deleteLater();
    });
    progressDialog->setFuture(future);
    progressDialog->exec();

    return result;
}

void TreeView::exportData(const QStandardItem *item, const QString &filePath,
                          bool hasTimeFormat, int headerSize)
{
    if (!item) {
        return;
    }

    //
    QString domain;
    int bitOffset = -1;

    if (item->type() == Icd::TreeItemTypeItemBitMap) {
        bitOffset = item->data(Icd::TreeBitOffsetRole).toInt();
        if (bitOffset < 0) {
            return;
        }
        //
        QStandardItem *parentItem = item->parent();
        if (!parentItem || parentItem->type() != Icd::TreeItemTypeDataItem) {
            return;
        }
        domain = parentItem->data(Icd::TreeItemDomainRole).toString();
    } else {
        domain = item->data(Icd::TreeItemDomainRole).toString();
    }

    if (domain.isEmpty()) {
        return;
    }

    auto &citer = d_fileTables.find(filePath);
    if (citer == d_fileTables.cend()) {
        return;
    }

    const Icd::TablePtr table = citer.value();
    if (!table) {
        return;
    }

    Icd::ObjectPtr objectItem = table->itemByDomain(domain.section('/', 3).toStdString());
    if(!objectItem) {
        return;
    }

    QString bitName;
    if (bitOffset != -1) {
        const Icd::BitItemPtr bitItem = JHandlePtrCast<Icd::BitItem, Icd::Object>(objectItem);
        if (!bitItem) {
            return;
        }
        bitName = QString("%1[bit%2]").arg(QString::fromStdString(bitItem->specAt(bitOffset)))
                .arg(bitOffset);
    }

    if (d_exportDataDlg) {
        d_exportDataDlg->deleteLater();
        d_exportDataDlg = nullptr;
    }

    d_exportDataDlg = new ExportDataDlg(filePath, table, objectItem, hasTimeFormat,
                                        headerSize, bitOffset,this);
    d_exportDataDlg->setWindowTitle(d_exportDataDlg->windowTitle() + tr("--")
                                    + (bitName.isEmpty() ? "" : bitName + '@')
                                    + generateTitle(objectItem)
                                    + '@' + QFileInfo(filePath).fileName());
    connect(d_exportDataDlg, &ExportDataDlg::finished, this, [=](){
        d_exportDataDlg->deleteLater();
        d_exportDataDlg = nullptr;
    });
    d_exportDataDlg->show();
}

QString TreeView::generateTitle(const Icd::ObjectPtr &item) const
{
    QString title;
    Icd::Object *tempObject = item.get();

    while (tempObject) {
        if (tempObject->objectType() == Icd::ObjectItem) {
            Icd::Item *tempItem = dynamic_cast<Icd::Item *>(tempObject);
            if (!tempItem) {
                break;
            }
            if (tempItem->type() == Icd::ItemComplex) {
                tempObject = tempObject->parent();
                continue;
            }
        }
        if (!title.isEmpty()) {
            title.append('@');
        }
        title.append(QString::fromStdString(tempObject->name()));
        tempObject = tempObject->parent();
    }

    if (title.isEmpty()) {
        return "<?>";
    }

    return title;
}

}
