#include "precomp.h"
#include "PreferenceView.h"
#include "channel/ChannelView.h"

namespace Preference {

View::View(QWidget *parent)
    : QDockWidget(parent)
{
    setObjectName(metaObject()->className());
    setWindowTitle(tr("Channel view"));
    setFeatures(features()
                | QDockWidget::DockWidgetMovable
                | QDockWidget::DockWidgetFloatable);

    QWidget *widget = new QWidget(this);
    widget->setObjectName("clientArea");
    setWidget(widget);

    QVBoxLayout *vertLayoutMain = new QVBoxLayout(widget);
    vertLayoutMain->setContentsMargins(0, 0, 0, 0);
    vertLayoutMain->setSpacing(0);

    d_channelView = new ChannelView(widget);
    vertLayoutMain->addWidget(d_channelView);
}

View::~View()
{
    JMain::saveWidgetState(this);
}

bool View::init()
{
    bool result = true;

    if (result) {
        JMain::restoreWidgetState(this);
    }

    result = result && d_channelView->init();

    return result;
}

}
