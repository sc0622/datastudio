#include "precomp.h"
#include "convertdatadlg.h"
#include "convertfromicdwidget.h"
#include "paritydatadlg.h"
#include "ConvertToDataAnalyse.h"
#include "converttoicdwidget.h"
#include "SplitFileDlg.h"

ConvertDataDlg::ConvertDataDlg(QWidget *parent)
    : QDialog(parent, Qt::WindowMinMaxButtonsHint | Qt::WindowCloseButtonHint)
{
    setWindowTitle(QStringLiteral("数据转换"));

    QVBoxLayout *vertLayoutMain = new QVBoxLayout(this);
    vertLayoutMain->setContentsMargins(3, 3, 3, 3);

    d_tabWidget = new QTabWidget(this);
    vertLayoutMain->addWidget(d_tabWidget);

    d_convFromIcdWidget = new ConvertFromIcdWidget(this);
    d_parityDataDlg = new ParityDataDlg(this);
    d_convtToDA = new ConvertToDataAnalyse(this);
    //d_convToIcdWidget = new ConvertToIcdWidget(this);
    d_splitFileDlg = new SplitFileDlg(this);

    d_tabWidget->addTab(d_convFromIcdWidget, QStringLiteral("去ICD数据化"));
    d_tabWidget->addTab(d_parityDataDlg, QStringLiteral("校验数据并转换"));
    d_tabWidget->addTab(d_convtToDA, QStringLiteral("转换为分析软件数据"));
//    d_tabWidget->addTab(d_convToIcdWidget, QStringLiteral("转换为ICD数据"));
    d_tabWidget->addTab(d_splitFileDlg, QStringLiteral("拆分数据文件"));

    adjustSize();

    resize(700, height());

    //
    connect(d_convFromIcdWidget, &ConvertFromIcdWidget::rejected, this, [=](){
        reject();
    });
    connect(d_parityDataDlg, &ParityDataDlg::rejected, this, [=](){
        reject();
    });/*
    connect(d_convToIcdWidget, &ConvertToIcdWidget::rejected, this, [=](){
        reject();
    });*/
    connect(d_convtToDA, &ConvertToDataAnalyse::rejected, this, [=](){
        reject();
    });
    connect(d_splitFileDlg, &SplitFileDlg::rejected, this, [=](){
        reject();
    });
}

ConvertDataDlg::~ConvertDataDlg()
{

}
