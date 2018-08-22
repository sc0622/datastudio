#include "precomp.h"
#include "DataSourceView.h"
#include "../DataSourceDlg.h"

DataSourceView::DataSourceView(QWidget *parent)
    : QWidget(parent)
{
    setProperty("__pane__", true);

    QVBoxLayout *layoutMain = new QVBoxLayout(this);
    layoutMain->setContentsMargins(0, 2, 0, 0);

    DataSourceWidget *dataSourceWidget = new DataSourceWidget("", nullptr, this);
    layoutMain->addWidget(dataSourceWidget);
}

DataSourceView::~DataSourceView()
{

}
