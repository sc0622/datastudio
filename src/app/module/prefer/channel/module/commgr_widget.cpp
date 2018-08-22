#include "precomp.h"
#include "commgr_widget.h"
#include "private/commgr_widget_p.h"

// class ComMgrWidget

ComMgrWidget::ComMgrWidget(QWidget *parent)
    : QWidget(parent)
    , J_DPTR(new ComMgrWidgetPrivate(this))
{
    Q_D(ComMgrWidget);
    d->init();
}

ComMgrWidget::~ComMgrWidget()
{
    Q_D(ComMgrWidget);
    delete d;
}

bool ComMgrWidget::init()
{
    Q_D(ComMgrWidget);

    if (!loadChannelConfig()) {
        return false;
    }

    d->updateUi();

    return true;
}

bool ComMgrWidget::loadChannelConfig()
{
    const Json::Value value = Json::resolve(JMain::instance()->configFile().toStdString(),
                                            "global.channel.filePath");
    if (value.isNull()) {
        return false;
    }

    if (!Icd::WorkerPool::getInstance()->loadConfig(value.asString())) {
        return false;
    }

    return true;
}
