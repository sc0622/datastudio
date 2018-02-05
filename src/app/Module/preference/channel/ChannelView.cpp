#include "precomp.h"
#include "ChannelView.h"
#include "module/commgr_widget.h"

namespace Preference {

ChannelView::ChannelView(QWidget *parent)
    : QWidget(parent)
{
    QVBoxLayout *vertLayoutMain = new QVBoxLayout(this);
    vertLayoutMain->setContentsMargins(0, 0, 0, 0);

    d_comMgrWidget = new ComMgrWidget(this);
    vertLayoutMain->addWidget(d_comMgrWidget);

    init();
}

ChannelView::~ChannelView()
{

}

bool ChannelView::init()
{
    bool result = true;

    result = result && d_comMgrWidget->init();

    return result;
}

}
