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
    , d_filePath(filePath)
    , d_table(table)
    , d_objectItem(objectItem)
    , d_hasTimeFormat(hasTimeFormat)
    , d_headerSize(headerSize)
    , d_bitOffset(bitOffset)
    , d_exportOrgDataWidget(nullptr)
    , d_convertToDataAnalyse(nullptr)
{
    setWindowTitle(QStringLiteral("导出数据"));

    QVBoxLayout *vertLayoutMain = new QVBoxLayout(this);
    vertLayoutMain->setContentsMargins(3, 3, 3, 3);

    d_tabWidget = new QTabWidget(this);
    vertLayoutMain->addWidget(d_tabWidget);

    //
    d_convertToDataAnalyse = new ConvertToDataAnalyse(
                filePath, table, objectItem, hasTimeFormat, headerSize, bitOffset, this);
    d_tabWidget->addTab(d_convertToDataAnalyse, QStringLiteral("导出为分析软件数据"));
    connect(d_convertToDataAnalyse, &ConvertToDataAnalyse::rejected, this, [=](){
        reject();
    });

    //
    if (bitOffset == -1) {
        d_exportOrgDataWidget = new ExportOrgDataWidget(
                    filePath, table, objectItem, hasTimeFormat, headerSize, bitOffset, this);
        d_tabWidget->addTab(d_exportOrgDataWidget, QStringLiteral("导出原始数据"));
        connect(d_exportOrgDataWidget, &ExportOrgDataWidget::rejected, this, [=](){
            reject();
        });
    }

    adjustSize();

    resize(1000, height() + 200);
}

ExportDataDlg::~ExportDataDlg()
{

}
