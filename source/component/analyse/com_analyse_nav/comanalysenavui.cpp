#include "precomp.h"
#include "comanalysenavui.h"
#include "mainnav_widget.h"

ComAnalyseNavUi::ComAnalyseNavUi(IJAttempter &attempter, QWidget *parent)
    : QWidget(parent)
    , d_attempter(attempter)
{
    QVBoxLayout *vertLayoutMain = new QVBoxLayout(this);
    vertLayoutMain->setContentsMargins(0, 0, 0, 0);

    d_navWidget = new MainNavWidget(this);
    vertLayoutMain->addWidget(d_navWidget);
}

ComAnalyseNavUi::~ComAnalyseNavUi()
{

}

bool ComAnalyseNavUi::init()
{
    bool result = true;

    //
    result = result && d_navWidget->init();

    return result;
}

void ComAnalyseNavUi::releaseWork()
{
    //
    d_navWidget->releaseWork();
}

void ComAnalyseNavUi::updateUi()
{
    d_navWidget->updateUi();
}

void ComAnalyseNavUi::showItemOffset(bool checked)
{
    d_navWidget->showItemOffset(checked);
}

void ComAnalyseNavUi::showItemType(bool checked)
{
    d_navWidget->showItemType(checked);
}

void ComAnalyseNavUi::loadIcdFile()
{
    d_navWidget->loadRecordData();
}

void ComAnalyseNavUi::unloadDataFile()
{
    d_navWidget->unloadRecordData();
}
