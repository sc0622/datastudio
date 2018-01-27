#include "precomp.h"
#include "mainnav_widget.h"
#include "icdwidget/coretree_widget.h"
#include "icdwidget/progressdialog.h"
#include "icdparser/icdparser.h"
#include "icdcore/icd_table.h"
#include "icdcore/icd_item_bit.h"
#include "icdworker/icdworker_util.h"
#include "tablesel_widget.h"
#include "axissel_widget.h"
#include "private/ExportDataDlg.h"

MainNavWidget::MainNavWidget(QWidget *parent)
    : QWidget(parent)
    , d_table(nullptr)
    , d_exportDataDlg(nullptr)
{
    QVBoxLayout *vertLayoutMain = new QVBoxLayout(this);
    vertLayoutMain->setContentsMargins(0, 0, 0, 0);

    d_icdTree = new Icd::CoreTreeWidget(this);
    d_icdTree->setTreeMode(Icd::CoreTreeWidget::TreeModeAnalyse);
    vertLayoutMain->addWidget(d_icdTree);

    //
    connect(d_icdTree, &Icd::CoreTreeWidget::itemClicked, this, [=]
            (QStandardItem *item){
        jframeNotifier().sendMessage("treeItemClick@com_analyse", (JWPARAM)item);
    });
    connect(d_icdTree, &Icd::CoreTreeWidget::currentItemChanged, this, [=]
            (QStandardItem *current, QStandardItem *previous){
        jframeNotifier().sendMessage("currentItemChanged@com_analyse",
                                     (JWPARAM)current, (JLPARAM)previous);
    });
    connect(d_icdTree, &Icd::CoreTreeWidget::itemUnloaded, this, [=]
            (QStandardItem *item, QStandardItem *itemTable){
        jframeNotifier().sendMessage("treeItemUnloaded@com_analyse",
                                     (JWPARAM)item, (JLPARAM)itemTable);
        if (item) {
            const QString filePath = item->data(Icd::TreeFilePathRole).toString();
            d_fileTables.remove(filePath);
            if (d_tempFiles.contains(filePath)) {
                d_tempFiles.remove(filePath);
            }
        }
    });
    connect(d_icdTree, &Icd::CoreTreeWidget::unbindItem, this, [=]
            (QStandardItem *item, QStandardItem *itemTable){
        //
        jframeNotifier().sendMessage("unbindItem@com_analyse",
                                     (JWPARAM)item, (JLPARAM)itemTable);
    });
    connect(d_icdTree, &Icd::CoreTreeWidget::exportAnalyseData, this, [=]
            (QStandardItem *item, const QString &filePath,
            bool hasTimeFormat, int headerSize){
        exportData(item, filePath, hasTimeFormat, headerSize);
    });
}

bool MainNavWidget::init()
{
    bool result = true;

    //
    Icd::Parser *parser = 0;
    if (jframeNotifier().sendMessage("query_icd_parser@com_icdsystem",
                                     (JWPARAM)&parser) != JFrame::ResultSuccess) {
        return false;
    }

    d_icdTree->setParser(parser);

    return result;
}

void MainNavWidget::releaseWork()
{
    d_fileTables.clear();
    d_tempFiles.clear();
}

void MainNavWidget::updateUi()
{
    //
    d_icdTree->setParser(0);
    Icd::Parser *parser = 0;
    if (jframeNotifier().sendMessage("query_icd_parser@com_icdsystem",
                                     (JWPARAM)&parser) != JFrame::ResultSuccess) {
        return;
    }
    d_icdTree->setParser(parser);

    //
    d_icdTree->clearContents();
}

void MainNavWidget::showItemOffset(bool checked)
{
    d_icdTree->setShowAttribute(Icd::CoreTreeWidget::ShowOffset, checked);
}

void MainNavWidget::showItemType(bool checked)
{
    d_icdTree->setShowAttribute(Icd::CoreTreeWidget::ShowType, checked);
}

void MainNavWidget::loadRecordData()
{
    QString selectedFilter = QStringLiteral("Any file (*.*)");
    const QString filePath = QFileDialog::getOpenFileName(
                this, QStringLiteral("加载数据"), QString(),
                "Any file (*.*);;"
                "ICD file (*.icd);;"
                "Data file (*.dat);;"
                "Text file (*.txt)", &selectedFilter);
    if (filePath.isEmpty()) {
        return;
    }

    JAutoCursor cursor(Qt::BusyCursor);

    //
    if (!parseDataFile(filePath)) {
        return;
    }
}

void MainNavWidget::unloadRecordData()
{
    d_icdTree->clearContents();
    d_table = Icd::TablePtr(nullptr);
    d_fileTables.clear();
    d_tempFiles.clear();
}

bool MainNavWidget::parseDataFile(const QString &filePath)
{
    QFile file(filePath);
    if (!file.open(QFile::ReadOnly)) {
        return false;
    }

    // [filetype:4B][attr:2B][domain:>=1B]
    if (file.size() < 7) {
        return false;   // 文件长度不正确
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

bool MainNavWidget::loadData(const QString &domain, const QString &filePath,
                             bool hasTimeFormat, int headerSize)
{
    bool result = true;

    if (!d_icdTree->parser()) {
        return false;
    }

    Icd::ProgressDialog *progressDialog = new Icd::ProgressDialog(this);
    progressDialog->setWindowTitle(QStringLiteral("加载数据"));
    progressDialog->setProgressValue(100);
    progressDialog->setCancelVisible(false);
    progressDialog->setMessage(QStringLiteral("正在解析数据文件……"));
    QFuture<bool> future = QtConcurrent::run([=]() -> bool {
        if (!d_icdTree->parser()->parseTable(domain.section('/', 0, 0).toStdString(),
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
                        QMessageBox::warning(this, QStringLiteral("警告"), QStringLiteral("解析失败！没有找到有效数据"));
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
                if (d_icdTree->loadData(d_table, newFilePath, hasTimeFormat,
                                        headerSize, domain)) {
                    //
                    d_fileTables[newFilePath] = d_table;
                    //
                    if (tempFile) {
                        d_tempFiles[newFilePath] = tempFile;
                    }
                    //
                    struct Data { Icd::TablePtr table; } data { d_table };
                    jframeNotifier().sendMessage("tableLoaded", (JWPARAM)&data, (JLPARAM)&newFilePath);
                }
                //
                d_table = Icd::TablePtr();
            }
        }
        progressDialog->disconnect(this);
        if (!progressDialog->isHidden()) {
            QString message;
            if (success) {
                message = QStringLiteral("解析成功！");
            } else {
                message = QStringLiteral("解析失败！");
            }
            progressDialog->hide();
            QMessageBox::information(this, QStringLiteral("解析结果"), message);
        }
        progressDialog->deleteLater();
    });
    progressDialog->setFuture(future);
    progressDialog->exec();

    return result;
}

void MainNavWidget::exportData(const QStandardItem *item, const QString &filePath,
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
    d_exportDataDlg->setWindowTitle(d_exportDataDlg->windowTitle() + QStringLiteral("——")
                                    + (bitName.isEmpty() ? "" : bitName + '@')
                                    + generateTitle(objectItem)
                                    + '@' + QFileInfo(filePath).fileName());
    connect(d_exportDataDlg, &ExportDataDlg::finished, this, [=](){
        d_exportDataDlg->deleteLater();
        d_exportDataDlg = nullptr;
    });
    d_exportDataDlg->show();
}

QString MainNavWidget::generateTitle(const Icd::ObjectPtr &item) const
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
        return QStringLiteral("<?>");
    }

    return title;
}
