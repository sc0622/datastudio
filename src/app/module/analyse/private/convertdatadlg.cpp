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
    setWindowTitle(tr("Data conversion"));

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

    tabWidget_->addTab(convFromIcdWidget_, tr("Remove ICD feature"));
    tabWidget_->addTab(parityDataDlg_, tr("Verfy data and convert"));
    tabWidget_->addTab(convtToDA_, tr("Convert to DataAnalyse"));
//    d_tabWidget->addTab(convToIcdWidget_, tr("Convert to ICD"));
    tabWidget_->addTab(splitFileDlg_, tr("Split data file"));

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
