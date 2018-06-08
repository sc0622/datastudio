#include "precomp.h"
#include "ExportDataDlg.h"
#include "ExportOrgDataWidget.h"
#include "ConvertToDataAnalyse.h"

ExportDataDlg::ExportDataDlg(const QString &filePath,
                             const Icd::TablePtr &table,
                             const Icd::ObjectPtr &objectItem,
                             bool hasTimeFormat, int headerSize,
                             int bitOffset, QWidget *parent)
    : QDialog(parent, Qt::WindowMinMaxButtonsHint | Qt::WindowCloseButtonHint)
    , filePath_(filePath)
    , table_(table)
    , objectItem_(objectItem)
    , hasTimeFormat_(hasTimeFormat)
    , headerSize_(headerSize)
    , bitOffset_(bitOffset)
    , exportOrgDataWidget_(nullptr)
    , convertToDataAnalyse_(nullptr)
{
    setWindowTitle(QStringLiteral("导出数据"));

    QVBoxLayout *vertLayoutMain = new QVBoxLayout(this);
    vertLayoutMain->setContentsMargins(0, 0, 0, 0);

    tabWidget_ = new QTabWidget(this);
    tabWidget_->setStyleSheet("QTabWidget::tab-bar{alignment:center;}");
    vertLayoutMain->addWidget(tabWidget_);
    //
    convertToDataAnalyse_ = new ConvertToDataAnalyse(
                filePath, table, objectItem, hasTimeFormat, headerSize, bitOffset, this);
    tabWidget_->addTab(convertToDataAnalyse_, QStringLiteral("导出为分析软件数据"));
    connect(convertToDataAnalyse_, &ConvertToDataAnalyse::rejected, this, [=](){
        reject();
    });
    //
    if (bitOffset == -1) {
        exportOrgDataWidget_ = new ExportOrgDataWidget(
                    filePath, table, objectItem, hasTimeFormat, headerSize, bitOffset, this);
        tabWidget_->addTab(exportOrgDataWidget_, QStringLiteral("导出原始数据"));
        connect(exportOrgDataWidget_, &ExportOrgDataWidget::rejected, this, [=](){
            reject();
        });
    }

    adjustSize();

    resize(1000, height() + 200);
}

ExportDataDlg::~ExportDataDlg()
{

}
