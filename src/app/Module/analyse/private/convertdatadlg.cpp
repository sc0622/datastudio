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
    setWindowTitle(QStringLiteral("����ת��"));

    QVBoxLayout *vertLayoutMain = new QVBoxLayout(this);
    vertLayoutMain->setContentsMargins(3, 3, 3, 3);

    d_tabWidget = new QTabWidget(this);
    vertLayoutMain->addWidget(d_tabWidget);

    d_convFromIcdWidget = new ConvertFromIcdWidget(this);
    d_parityDataDlg = new ParityDataDlg(this);
    d_convtToDA = new ConvertToDataAnalyse(this);
    //d_convToIcdWidget = new ConvertToIcdWidget(this);
    d_splitFileDlg = new SplitFileDlg(this);

    d_tabWidget->addTab(d_convFromIcdWidget, QStringLiteral("ȥICD���ݻ�"));
    d_tabWidget->addTab(d_parityDataDlg, QStringLiteral("У�����ݲ�ת��"));
    d_tabWidget->addTab(d_convtToDA, QStringLiteral("ת��Ϊ�����������"));
//    d_tabWidget->addTab(d_convToIcdWidget, QStringLiteral("ת��ΪICD����"));
    d_tabWidget->addTab(d_splitFileDlg, QStringLiteral("��������ļ�"));

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
