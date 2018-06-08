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
    vertLayoutMain->setContentsMargins(0, 0, 0, 0);

    tabWidget_ = new QTabWidget(this);
    tabWidget_->setStyleSheet("QTabWidget::tab-bar{alignment:center;}");
    vertLayoutMain->addWidget(tabWidget_);

    convFromIcdWidget_ = new ConvertFromIcdWidget(this);
    parityDataDlg_ = new ParityDataDlg(this);
    convtToDA_ = new ConvertToDataAnalyse(this);
    //convToIcdWidget_ = new ConvertToIcdWidget(this);
    splitFileDlg_ = new SplitFileDlg(this);

    tabWidget_->addTab(convFromIcdWidget_, QStringLiteral("去ICD数据化"));
    tabWidget_->addTab(parityDataDlg_, QStringLiteral("校验数据并转换"));
    tabWidget_->addTab(convtToDA_, QStringLiteral("转换为分析软件数据"));
//    d_tabWidget->addTab(convToIcdWidget_, QStringLiteral("转换为ICD数据"));
    tabWidget_->addTab(splitFileDlg_, QStringLiteral("拆分数据文件"));

    adjustSize();

    resize(700, height());

    //
    connect(convFromIcdWidget_, &ConvertFromIcdWidget::rejected, this, [=](){
        reject();
    });
    connect(parityDataDlg_, &ParityDataDlg::rejected, this, [=](){
        reject();
    });/*
    connect(convToIcdWidget_, &ConvertToIcdWidget::rejected, this, [=](){
        reject();
    });*/
    connect(convtToDA_, &ConvertToDataAnalyse::rejected, this, [=](){
        reject();
    });
    connect(splitFileDlg_, &SplitFileDlg::rejected, this, [=](){
        reject();
    });
}

ConvertDataDlg::~ConvertDataDlg()
{

}
